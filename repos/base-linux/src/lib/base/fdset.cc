/*
 * \brief  File/socket descriptor set for select/poll
 * \author Stefan Thoeni
 * \date   2019-12-13
 */

/*
 * Copyright (C) 2019 gapfruit AG
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <base/log.h>
#include <base/ipc.h>

/* base-internal includes */
#include <base/internal/rpc_destination.h>
#include <base/internal/rpc_obj_key.h>
#include <base/internal/capability_space_tpl.h>
#include <base/internal/fdset.h>

using namespace Genode;

Fd_set::Fd_set()
{
}

void Fd_set::clear()
{
	_num_fds = 0;
	Genode::memset(_fds, 0, sizeof(_fds));

	enum { LOCAL_SOCKET = 0, REMOTE_SOCKET = 1 };
	int sd[2];
	sd[LOCAL_SOCKET] = -1; sd[REMOTE_SOCKET] = -1;

	int ret = lx_socketpair(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0, sd);
	if (ret < 0) {
		raw(lx_getpid(), ":", lx_gettid(), " lx_socketpair failed with ", ret);
		throw Genode::Ipc_error();
	}

	_cancel_client_sd = sd[REMOTE_SOCKET];
	_cancel_server_sd = sd[LOCAL_SOCKET];
	add(_cancel_server_sd);
}

void Fd_set::add(int fd)
{
	for (int i = 0; i < _num_fds; i++) {
		if (_fds[i].fd < 0) {
			_fds[i].fd = fd;
			_fds[i].events = POLLIN;
			_fds[i].revents = 0;
			return;
		}
	}

	if (_num_fds < MAX_FDS) {
		_fds[_num_fds].fd = fd;
		_fds[_num_fds].events = POLLIN;
		_fds[_num_fds].revents = 0;
		_num_fds++;
	} else {
		raw(lx_getpid(), ":", lx_gettid(), " out of file descriptors on add");
		throw Genode::Ipc_error();
	}
}

void Fd_set::remove(int fd)
{
	for (int i = 0; i < _num_fds; i++) {
		if (_fds[i].fd == fd) {
			_fds[i].fd = -1;
			_fds[i].events = 0;
			_fds[i].revents = 0;
			return;
		}
	}

	raw(lx_getpid(), ":", lx_gettid(), " file descriptor not in set on remove");
}

int Fd_set::poll()
{
	for (int i = 0; i < _num_fds; i++) {
		if (_fds[i].fd > 0 && _fds[i].revents != 0) {
			_fds[i].revents = 0;
			return _fds[i].fd;
		}
	}

	int result = lx_poll(_fds, _num_fds, -1);
	if (result == 0) {
		raw(lx_getpid(), ":", lx_gettid(), " unexpected timeout from lx_poll on poll");
		return -1;
	} else if (result < 0) {
		raw(lx_getpid(), ":", lx_gettid(), " lx_poll failed on poll with error", result);
		return -1;
	}

	for (int i = 0; i < _num_fds; i++) {
		switch (_fds[i].revents) {
			case 0:
				break;
			case POLLIN:
				_fds[i].revents = 0;
				return _fds[i].fd;
			default:
				raw(lx_getpid(), ":", lx_gettid(), " unknown revent ", _fds[i].revents, " from lx_poll on poll");
				_fds[i].revents = 0;
				return -1;
		}
	}

	return -1;
}

void Fd_set::write_cancel()
{
	msghdr msg;
	Genode::memset(&msg, 0, sizeof(msghdr));
	int const ret = lx_sendmsg(_cancel_client_sd, &msg, 0);
	if (ret < 0) {
		raw(lx_getpid(), ":", lx_gettid(), " write_cancel ep_nd=", this, " cc_sd(", &_cancel_client_sd, ")=", _cancel_client_sd, " lx_sendmsg failed ", ret);
		throw Genode::Ipc_error();
	}
}

bool Fd_set::clear_cancel(int fd)
{
	if (fd == _cancel_server_sd) {
		msghdr msg;
		Genode::memset(&msg, 0, sizeof(msghdr));
		int const ret = lx_recvmsg(_cancel_server_sd, &msg, 0);
		if (ret < 0) {
			raw(lx_getpid(), ":", lx_gettid(), " read_cancel lx_recvmsg failed ", ret);
			throw Genode::Ipc_error();
		}
		return false;
	} else {
		return true;
	}
}

