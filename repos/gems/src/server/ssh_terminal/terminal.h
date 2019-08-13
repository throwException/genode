/*
 * \brief  Component providing a Terminal session via SSH
 * \author Josef Soentgen
 * \author Pirmin Duss
 * \date   2019-05-29
 */

/*
 * Copyright (C) 2018 Genode Labs GmbH
 * Copyright (C) 2019 gapfruit AG
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SSH_TERMINAL_TERMINAL_H_
#define _SSH_TERMINAL_TERMINAL_H_

/* Genode includes */
#include <base/capability.h>
#include <base/signal.h>
#include <session/session.h>
#include <base/log.h>
#include <terminal_session/terminal_session.h>
#include <timer_session/connection.h>

/* local includes */
#include "login.h"

/* gems includes */
#include <gems/magic_ring_buffer.h>

namespace Ssh
{
	using namespace Genode;

	class Terminal;
}


class Ssh::Terminal
{
	public:

		int  write_avail_fd { -1 };
		bool raw_mode       { false };

	private:

		Genode::Env &_env;
		::Terminal::Session::Size _size { 0, 0 };

		Signal_context_capability _size_changed_sigh;
		Signal_context_capability _connected_sigh;
		Signal_context_capability _read_avail_sigh;

		Ssh::User const _user { };


		Genode::Lock _read_lock            { };
		Genode::Lock _write_lock           { };

		Magic_ring_buffer<char> _read_buf  { _env, 16*1024 };
		Magic_ring_buffer<char> _write_buf { _env, 16*1024 };

		unsigned _attached_channels { 0u };
		unsigned _pending_channels  { 0u };

	public:

		/**
		 * Constructor
		 */
		Terminal(Ssh::User const &user, Genode::Env &env) : _env(env), _user(user) { }

		virtual ~Terminal() = default;

		Ssh::User const &user() const { return _user; }

		unsigned attached_channels() const { return _attached_channels; }

		void attach_channel() { ++_attached_channels; }
		void detach_channel() { --_attached_channels; }
		void reset_pending()  { _pending_channels = 0; }

		/*********************************
		 ** Terminal::Session interface **
		 *********************************/

		/**
		 * Register signal handler to be notified once the size was changed
		 */
		void size_changed_sigh(Signal_context_capability sigh) {
			_size_changed_sigh = sigh; }

		/**
		 * Register signal handler to be notified once we accepted the TCP
		 * connection
		 */
		void connected_sigh(Signal_context_capability sigh)
		{
			_connected_sigh = sigh;

			if (_attached_channels > 0) {
				notify_connected();
			}
		}

		/**
		 * Register signal handler to be notified when data is available for
		 * reading
		 */
		void read_avail_sigh(Signal_context_capability sigh)
		{
			_read_avail_sigh = sigh;

			/* if read data is available right now, deliver signal immediately */
			if (read_avail() && _read_avail_sigh.valid()) {
				Signal_transmitter(_read_avail_sigh).submit();
			}
		}

		/**
		 * Inform client about the finished initialization of the SSH
		 * session
		 */
		void notify_connected()
		{
			if (!_connected_sigh.valid()) { return; }
			Signal_transmitter(_connected_sigh).submit();
		}

		/**
		 * Inform client about avail data
		 */
		void notify_read_avail()
		{
			if (!_read_avail_sigh.valid()) { return; }
			Signal_transmitter(_read_avail_sigh).submit();
		}

		/**
		 * Inform client about the changed size of the remote terminal
		 */
		void notify_size_changed()
		{
			if (!_size_changed_sigh.valid()) { return; }
			Signal_transmitter(_size_changed_sigh).submit();
		}

		/**
		 * Set size of the Terminal session to match remote terminal
		 */
		void size(::Terminal::Session::Size size) { _size = size; }

		/**
		 * Return size of the Terminal session
		 */
		::Terminal::Session::Size size() const { return _size; }

		/*****************
		 ** I/O methods **
		 *****************/

		/**
		 * Add received data to ring buffer
		 */
		size_t receive_data(char const *src, Genode::size_t src_len)
		{
			size_t in_bytes { 0 };
			size_t write_avail { 0 };

			{
				Lock::Guard g { _read_lock };
				const size_t max_bytes { Genode::min(src_len, _read_buf.write_avail()) };

				if (!max_bytes) {
					return max_bytes;
				}

				char *dst = _read_buf.write_addr();

				if (raw_mode) {
					Genode::memcpy(dst, src, max_bytes);
					_read_buf.fill(max_bytes);
					in_bytes = max_bytes;
				} else {
					while ( in_bytes < max_bytes ) {

						char c = src[in_bytes];

						/* replace ^? with ^H */
						enum { DEL = 0x7f, BS = 0x08, };
						if (c == DEL) {
							dst[in_bytes] = BS;
						} else {
							dst[in_bytes] = c;
						}
						in_bytes++;
					}
					_read_buf.fill(in_bytes);

					write_avail = _read_buf.write_avail();
				}

				notify_read_avail();
			}

			if (!write_avail) {
				/* waiting for terminal session to read some data*/
				uint8_t timeout = 10u;
				while (write_avail < 1024) {
					/* FIXME: At this moment we just filled our buffer from data
					 * we received over the wire with libssh and we give noux
					 * time to read (notify noux and let noux pull data via IPC).
					 */
					usleep(500000);
					if (--timeout == 0) {
						Genode::error("timeout while waiting for read");
						break;
					}
					{
						Lock::Guard g { _read_lock };
						write_avail = _read_buf.write_avail();
					}
				}
			}

			return in_bytes;
		}

		/**
		 * Send internal write buffer content to SSH channel
		 */
		void send(ssh_channel channel)
		{
			Lock::Guard g { _write_lock };

			if (!_write_buf.read_avail()) { return; }

			/* ignore send request */
			if (!channel || !ssh_channel_is_open(channel)) { return; }

			char const *src     = _write_buf.read_addr();
			size_t const len    = _write_buf.read_avail();
			/* XXX we do not handle partial writes */
			int const num_bytes = ssh_channel_write(channel, src, len);

			_write_buf.drain(num_bytes);

			if (num_bytes && (size_t)num_bytes < len) {
				warning("send on channel was truncated");
			}

			if (++_pending_channels >= _attached_channels) {
				/* reset buffer */
				_write_buf.drain(_write_buf.read_avail());
			}

			/* at this point the client might have disconnected */
			if (num_bytes < 0) { throw -1; }
		}

		/******************************************
		 ** Methods called by Terminal front end **
		 ******************************************/

		/**
		 * Read out internal read buffer and copy into destination buffer.
		 */
		size_t read(char *dst, size_t dst_len)
		{
			Genode::Lock::Guard g { _read_lock };
			const size_t num_bytes { min(dst_len, _read_buf.read_avail()) };
			Genode::memcpy(dst, _read_buf.read_addr(), num_bytes);
			_read_buf.drain(num_bytes);

			/* notify client if there are still bytes available for reading */
			if (_read_buf.read_avail()) {
				notify_read_avail();
			}

			return num_bytes;
		}

		/**
		 * Write into internal buffer and copy to underlying socket
		 */
		size_t write(char const *src, Genode::size_t src_len)
		{
			Lock::Guard g { _write_lock };
			size_t in_bytes { 0 };
			const size_t max_bytes { Genode::min(src_len, _write_buf.write_avail()) };

			if (!max_bytes) {
				return 0;
			}

			if (raw_mode) {
				char *dst = _write_buf.write_addr();
				Genode::memcpy(dst, src, max_bytes);
				_write_buf.fill(max_bytes);
				in_bytes = max_bytes;
			} else {
				size_t out_bytes = 0;
				while ( out_bytes < max_bytes ) {

					char c = src[in_bytes];
					if (c == '\n') {
						_write_buf.write_addr()[out_bytes] = '\r';
						out_bytes++;
					}

					_write_buf.write_addr()[out_bytes] = c;
					out_bytes++;
					in_bytes++;
				}
				_write_buf.fill(out_bytes);
			}
			_wake_loop();

			return in_bytes;
		}

		/**
		 * Return true if the internal read buffer is ready to receive data
		 */
		bool read_avail()
		{
			Genode::Lock::Guard g { _read_lock };
			return _read_buf.read_avail() != 0;
		}

private:
		void _wake_loop()
		{
			Libc::with_libc([&] {
				/* wake the event loop up */
				char c = 1;
				::write(write_avail_fd, &c, sizeof(c));
			});
			Timer::Connection timer {_env};
			timer.msleep(1);
		}
};

#endif  /* _SSH_TERMINAL_TERMINAL_H_ */
