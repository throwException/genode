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

#ifndef _INCLUDE__BASE__INTERNAL__FDSET_H_
#define _INCLUDE__BASE__INTERNAL__FDSET_H_

#include <base/rpc_server.h>
#include <linux_syscalls.h>

namespace Genode {

	class Fd_set : public Genode::Native_context
	{
		protected:
			Fd_set() { }

		public:
			virtual void clear() = 0;

			virtual void add(int fd) = 0;

			virtual void remove(int fd) = 0;

			virtual int poll() = 0;

			virtual void write_cancel() = 0;

			virtual bool clear_cancel(int fd) = 0;

			virtual ~Fd_set()
			{
			}
	};

	class Fd_set_impl : public Fd_set
	{
		private:
			int _cancel_client_sd = 0;
			int _cancel_server_sd = 0;
			int _epoll_fd = 0;

		public:
			Fd_set_impl()
			{
			}

			void clear() override
			{
				_epoll_fd = lx_epoll_create();
        if (_epoll_fd < 0) {
          raw(lx_getpid(), ":", lx_gettid(), " lx_epoll_create failed with ", _epoll_fd);
          throw Genode::Ipc_error();
        }

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

			void add(int fd) override
			{
				epoll_event event;
				event.events = EPOLLIN;
				event.data.fd = fd;
				int ret = lx_epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event);
				if (ret < 0) {
					raw(lx_getpid(), ":", lx_gettid(), " lx_epoll_ctl add failed with ", ret);
					throw Genode::Ipc_error();
				}
			}

			void remove(int fd) override
			{
				epoll_event event;
				event.events = EPOLLIN;
				event.data.fd = fd;
				int ret = lx_epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &event);
				if (ret == -2) {
					/* ignore file already closed */
				} else if (ret == -9) {
					/* ignore file already closed */
				} else if (ret < 0) {
					raw(lx_getpid(), ":", lx_gettid(), " lx_epoll_ctl remove failed with ", ret);
					throw Genode::Ipc_error();
				}
			}

			int poll() override
			{
				epoll_event events[1];
				int event_count = lx_epoll_wait(_epoll_fd, events, 1, -1);
				if (event_count < 0) {
					raw(lx_getpid(), ":", lx_gettid(), " lx_epoll_ctl failed with ", event_count);
					throw Genode::Ipc_error();
				}
				else if (event_count == 0)
				{
					return -1;
				}
				else if (event_count == 1)
				{
					auto e = events[0].events;
					switch (e) {
						case 0:
							return -1;
						case POLLIN:
							return events[0].data.fd;
						default:
							raw(lx_getpid(), ":", lx_gettid(), " unknown revent ", e, " from epoll_wait");
							return -1;
					}
				}
				else
				{
					raw(lx_getpid(), ":", lx_gettid(), " to many event on epoll_wait");
					throw Genode::Ipc_error();
				}
			}

			void write_cancel() override
			{
				msghdr msg;
				Genode::memset(&msg, 0, sizeof(msghdr));
				int const ret = lx_sendmsg(_cancel_client_sd, &msg, 0);
				if (ret < 0) {
					raw(lx_getpid(), ":", lx_gettid(), " write_cancel ep_nd=", this, " cc_sd(", &_cancel_client_sd, ")=", _cancel_client_sd, " lx_sendmsg failed ", ret);
					throw Genode::Ipc_error();
				}
			}

			bool clear_cancel(int fd) override
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
	};
}

#endif /* _INCLUDE__BASE__INTERNAL__FDSET_H_ */
