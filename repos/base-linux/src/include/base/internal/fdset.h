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

#include <linux_syscalls.h>

namespace Genode {

	enum {
		MAX_FDS = 4096
	};

	class Fd_set {

		private:
			int _cancel_client_sd = 0;
			int _cancel_server_sd = 0;
			pollfd _fds[MAX_FDS];
			int _num_fds = 0;

		public:
			Fd_set();

			void clear();

			void add(int fd);

			void remove(int fd);

			int poll();

			void write_cancel();

			bool clear_cancel(int sd);

	};
}

#endif /* _INCLUDE__BASE__INTERNAL__FDSET_H_ */
