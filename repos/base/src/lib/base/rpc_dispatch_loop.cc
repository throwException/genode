/*
 * \brief  Default version of platform-specific part of RPC framework
 * \author Norman Feske
 * \date   2006-05-12
 */

/*
 * Copyright (C) 2006-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <util/retry.h>
#include <base/rpc_server.h>

/* base-internal includes */
#include <base/internal/ipc_server.h>

using namespace Genode;

void Rpc_entrypoint::_entry(Native_context* native_context)
{
	_native_context = native_context;

	Ipc_server srv(*this);
	_cap = srv;
	_cap_valid.unlock();

	/*
	 * Now, the capability of the server activation is initialized
	 * an can be passed around. However, the processing of capability
	 * invocations should not happen until activation-using server
	 * is completely initialized. Thus, we wait until the activation
	 * gets explicitly unblocked by calling 'Rpc_entrypoint::activate()'.
	 */
	_delay_start.lock();

	Rpc_exception_code exc = Rpc_exception_code(Rpc_exception_code::INVALID_OBJECT);

	while (!_exit_handler.exit) {

		Rpc_request const request = ipc_reply_wait(_caller, exc, _snd_buf, _rcv_buf, *this);
		_caller = request.caller;

		Ipc_unmarshaller unmarshaller(_rcv_buf);
		Rpc_opcode opcode(0);
		unmarshaller.extract(opcode);

		/* set default return value */
		exc = Rpc_exception_code(Rpc_exception_code::INVALID_OBJECT);
		_snd_buf.reset();

		apply(request.badge, [&] (Rpc_object_base *obj)
		{
			if (!obj) { return;}
			try { exc = obj->dispatch(opcode, unmarshaller, _snd_buf); }
			catch(Blocking_canceled&) { }
		});
	}

	/* answer exit call, thereby wake up '~Rpc_entrypoint' */
	Msgbuf<16> snd_buf;
	ipc_reply(_caller, Rpc_exception_code(Rpc_exception_code::SUCCESS), snd_buf);

	/* defer the destruction of 'Ipc_server' until '~Rpc_entrypoint' is ready */
	_delay_exit.lock();
}
