/*
 * \brief  Allocate an RPC capability via socketpair on Linux
 * \author Stefan Thoeni
 * \date   2020-01-30
 */

/*
 * Copyright (C) 2020 gapfruit AG
 * Copyright (C) 2020 Genode Labs GmbH
 */

#ifndef _INCLUDE__BASE__INTERNAL__RPC_CAP_ALLOC_SOCKETPAIR_
#define _INCLUDE__BASE__INTERNAL__RPC_CAP_ALLOC_SOCKETPAIR_

/* Genode includes */
#include <base/env.h>

/* base-internal includes */
#include <base/internal/capability_space_tpl.h>
#include <base/internal/fdset.h>

namespace Genode
{
	Native_capability _alloc_rpc_cap_socketpair(Fd_set* set);

	void _free_rpc_cap_socketpair(Fd_set* set, Native_capability cap);
}

#endif /* _INCLUDE__BASE__INTERNAL__RPC_CAP_ALLOC_SOCKETPAIR_ */
