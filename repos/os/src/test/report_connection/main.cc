/*
 * \brief  Test for report-ROM service
 * \author Norman Feske
 * \date   2013-01-10
 */

/*
 * Copyright (C) 2014-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/log.h>
#include <base/component.h>
#include <base/attached_rom_dataspace.h>
#include <os/reporter.h>
#include <timer_session/connection.h>


#define ASSERT(cond) \
	if (!(cond)) { \
		error("assertion ", #cond, " failed"); \
		throw -2; }


namespace Test {
	struct Main;
	using namespace Genode;
}


struct Test::Main
{
	Env &_env;
	Timer::Connection _timer { _env };
	int _value = 0;

	Expanding_reporter _reporter { _env, "config", "brightness" };

	void _handle_timer()
	{
		log("reporting...");
		_value++;
		_reporter.generate([&] (Xml_generator &xml) {
			xml.attribute("value", _value);
		});
		log("reported: ", _value);
	}

	Signal_handler<Main> _timer_handler {
		_env.ep(), *this, &Main::_handle_timer };

	Main(Env &env) : _env(env)
	{
		log("starting...");
		_timer.sigh(_timer_handler);
		_timer.trigger_periodic(3000000);
	}
};


void Component::construct(Genode::Env &env) { static Test::Main main(env); }
