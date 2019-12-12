/*
 * \brief  I/O channel targeting Genode's terminal interface
 * \author Norman Feske
 * \date   2011-10-21
 */

/*
 * Copyright (C) 2011-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _NOUX__TERMINAL_IO_CHANNEL_H_
#define _NOUX__TERMINAL_IO_CHANNEL_H_

/* Genode includes */
#include <util/string.h>
#include <base/log.h>
#include <os/ring_buffer.h>
#include <terminal_session/connection.h>

/* Noux includes */
#include <io_channel.h>
#include <noux_session/sysio.h>

namespace Noux { struct Terminal_io_channel; }


struct Noux::Terminal_io_channel : Io_channel
{
	Terminal::Session &_terminal;

	Signal_handler<Terminal_io_channel> _read_avail_handler;

	Signal_handler<Terminal_io_channel> _resize_handler;

	enum { EOF = 4 };

	bool eof = false;

	enum Type { STDIN, STDOUT, STDERR } type;

	Ring_buffer<char, Sysio::CHUNK_SIZE + 1> read_buffer { };

	char ipc_read_buffer[1024u];

	Terminal_io_channel(Terminal::Session &terminal, Type type,
	                    Entrypoint &ep)
	:
		_terminal(terminal),
		_read_avail_handler(ep, *this, &Terminal_io_channel::_handle_read_avail),
		_resize_handler    (ep, *this, &Terminal_io_channel::_handle_resize),
		type(type)
	{
		/*
		 * Enable wake up STDIN channel on the presence of new input
		 *
		 * By registering our I/O channel as signal handler, the Noux
		 * main loop will be unblocked on the arrival of new input.
		 * It will check if the received signal belongs to an I/O channel
		 * and invokes the 'handle_signal' function of the I/O channel.
		 *
		 * This gives us the opportunity to handle the unblocking of
		 * blocking system calls such as 'select'.
		 */
		if (type == STDIN) {
			terminal.read_avail_sigh(_read_avail_handler);
			terminal.size_changed_sigh(_resize_handler);
		}
	}

	bool write(Sysio &sysio) override
	{
		size_t const count = min(sysio.write_in.count,
		                         sizeof(sysio.write_in.chunk));

		sysio.write_out.count = _terminal.write(sysio.write_in.chunk, count);

		return true;
	}

	bool read(Sysio &sysio) override
	{
		if (type != STDIN) {
			error("attempt to read from terminal output channel");
			return false;
		}

		/* deliver EOF observed by the previous 'read' call */
		if (eof) {
			sysio.read_out.count = 0;
			eof = false;
			return true;
		}

		size_t const max_count =
			min(sysio.read_in.count,
			    sizeof(sysio.read_out.chunk));

		for (sysio.read_out.count = 0;
		     (sysio.read_out.count < max_count) && !read_buffer.empty();
		     sysio.read_out.count++) {

			char c = read_buffer.get();

			if (c == EOF) {

				/*
				 * If EOF was the only character of the batch, the count
				 * has reached zero. In this case the read result indicates
				 * the EOF condition as is. However, if count is greater
				 * than zero, we deliver the previous characters of the
				 * batch and return the zero result from the subsequent
				 * 'read' call. This condition is tracked by the 'eof'
				 * variable.
				 */
				if (sysio.read_out.count > 0)
					eof = true;

				return true;
			}

			sysio.read_out.chunk[sysio.read_out.count] = c;
		}

		return true;
	}

	bool fcntl(Sysio &sysio) override
	{
		/**
		 * Actually it is "inappropiate" to use fcntl() directly on terminals
		 * (atleast according to the Open Group Specification). We do it anyway
		 * since in our case stdout/in/err is directly connected to the terminal.
		 *
		 * Some GNU programms check if stdout is open by calling fcntl(stdout, F_GETFL, ...).
		 */
		switch (sysio.fcntl_in.cmd) {

		case Sysio::FCNTL_CMD_GET_FILE_STATUS_FLAGS:
			sysio.fcntl_out.result = 0;
			return true;

		default:
			return false;
		}

		return false;
	}

	bool fstat(Sysio &sysio) override
	{
		sysio.fstat_out.st.type = Vfs::Node_type::CONTINUOUS_FILE;
		return true;
	}

	bool check_unblock(bool rd, bool wr, bool) const override
	{
		/* never block for writing */
		if (wr) return true;

		/*
		 * Unblock I/O channel if the terminal has new user input. Channels
		 * otther than STDIN will never unblock.
		 */
		return (rd && (type == STDIN) && !read_buffer.empty());
	}

	bool ioctl(Sysio &sysio) override
	{
		switch (sysio.ioctl_in.request) {

		case Vfs::File_io_service::IOCTL_OP_TIOCGWINSZ:
			{
				Terminal::Session::Size size = _terminal.size();
				sysio.ioctl_out.tiocgwinsz.rows    = size.lines();
				sysio.ioctl_out.tiocgwinsz.columns = size.columns();
				return true;
			}

		case Vfs::File_io_service::IOCTL_OP_TIOCSETAF:
			{
				warning(__func__, ": OP_TIOCSETAF not implemented");
				return false;
			}

		case Vfs::File_io_service::IOCTL_OP_TIOCSETAW:
			{
				warning(__func__, ": OP_TIOCSETAW not implemented");
				return false;
			}

		default:

			warning("invalid ioctl request ", (int)sysio.ioctl_in.request);
			return false;
		};
	}

	void _handle_read_avail()
	{
		if (_terminal.avail()) {
			const size_t max_count = Genode::min(static_cast<size_t>
			                               (read_buffer.avail_capacity()),
			                               sizeof(ipc_read_buffer));

			const size_t num_bytes = _terminal.read(ipc_read_buffer, max_count);

			for (size_t i = 0; i < num_bytes; ++i) {
				char c = ipc_read_buffer[i];

				enum { INTERRUPT = 3 };

				if (c == INTERRUPT) {
					Io_channel::invoke_all_interrupt_handlers(Sysio::SIG_INT);
				} else {
					read_buffer.add(c);
				}
			}
		}

		Io_channel::invoke_all_notifiers();
	}

	void _handle_resize()
	{
		/* respond to terminal-close event */
		Terminal::Session::Size const size = _terminal.size();
		if (size.columns()*size.lines() == 0)
			read_buffer.add(EOF);

		Io_channel::invoke_all_interrupt_handlers(Sysio::SIG_WINCH);
	}
};

#endif /* _NOUX__TERMINAL_IO_CHANNEL_H_ */
