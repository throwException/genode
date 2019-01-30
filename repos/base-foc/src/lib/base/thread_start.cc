/*
 * \brief  Fiasco-specific implementation of the non-core startup Thread API
 * \author Norman Feske
 * \author Stefan Kalkowski
 * \author Martin Stein
 * \date   2010-01-19
 */

/*
 * Copyright (C) 2010-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <base/thread.h>
#include <base/sleep.h>
#include <base/env.h>
#include <cpu_thread/client.h>
#include <foc/native_capability.h>
#include <foc_native_cpu/client.h>
#include <deprecated/env.h>

/* base-internal includes */
#include <base/internal/stack.h>
#include <base/internal/cap_map.h>
#include <base/internal/globals.h>

/* Fiasco includes */
namespace Fiasco {
#include <l4/sys/utcb.h>
}

using namespace Genode;


void Thread::_deinit_platform_thread()
{
	using namespace Fiasco;

	if (native_thread().kcap && _thread_cap.valid()) {
		Cap_index *i = (Cap_index*)l4_utcb_tcr_u(utcb()->foc_utcb)->user[UTCB_TCR_BADGE];
		cap_map().remove(i);
		_cpu_session->kill_thread(_thread_cap);
	}
}


void Thread::_init_platform_thread(size_t weight, Type type)
{
	_init_cpu_session_and_trace_control();

	if (type == NORMAL)
	{
		/* create thread at core */
		_thread_cap = _cpu_session->create_thread(env_deprecated()->pd_session_cap(),
		                                          name(), _affinity,
		                                          Weight(weight));

		/* assign thread to protection domain */
		if (!_thread_cap.valid())
			throw Cpu_session::Thread_creation_failed();

		return;
	}
	/* adjust values whose computation differs for a main thread */
	native_thread().kcap = Fiasco::MAIN_THREAD_CAP;
	_thread_cap = main_thread_cap();

	if (!_thread_cap.valid())
		throw Cpu_session::Thread_creation_failed();

	/* make thread object known to the Fiasco environment */
	addr_t const t = (addr_t)this;
	Fiasco::l4_utcb_tcr()->user[Fiasco::UTCB_TCR_THREAD_OBJ] = t;
}


void Thread::start()
{
	using namespace Fiasco;

	Foc_native_cpu_client native_cpu(_cpu_session->native_cpu());

	/* get gate-capability and badge of new thread */
	Foc_thread_state state;
	try { state = native_cpu.thread_state(_thread_cap); }
	catch (...) { throw Cpu_session::Thread_creation_failed(); }

	/* remember UTCB of the new thread */
	Fiasco::l4_utcb_t * const foc_utcb = (Fiasco::l4_utcb_t *)state.utcb;
	utcb()->foc_utcb = foc_utcb;

	native_thread() = Native_thread(state.kcap);

	Cap_index *i = cap_map().insert(state.id, state.kcap);
	l4_utcb_tcr_u(foc_utcb)->user[UTCB_TCR_BADGE]      = (unsigned long) i;
	l4_utcb_tcr_u(foc_utcb)->user[UTCB_TCR_THREAD_OBJ] = (addr_t)this;

	/* register initial IP and SP at core */
	Cpu_thread_client cpu_thread(_thread_cap);
	cpu_thread.start((addr_t)_thread_start, _stack->top());
}


void Thread::cancel_blocking()
{
	Cpu_thread_client(_thread_cap).cancel_blocking();
}
