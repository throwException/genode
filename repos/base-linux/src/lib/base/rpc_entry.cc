/*
 * \brief  Default version of platform-specific part of RPC framework
 * \author Stefan Thöni
 * \date   2020-01-30
 */

/*
 * Copyright (C) 2006-2020 Genode Labs GmbH
 * Copyright (C) 2020 gapfruit AG
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <base/rpc_server.h>

/* base-internal includes */
#include <base/internal/ipc_server.h>
#include <base/internal/fdset.h>

using namespace Genode;

void Rpc_entrypoint::entry()
{
	Fd_set_impl fd_set { };
	_entry(&fd_set);
}

size_t Genode::_native_stack_size(size_t stack_size)
{
	return stack_size + sizeof(Fd_set_impl);
}

