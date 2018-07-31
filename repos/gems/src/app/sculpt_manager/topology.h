/*
 * \brief  Qumela topology management
 * \author Roman Iten
 * \date   2018-07-30
 */

/*
 * Copyright (C) 2018 gapfruit AG
 */

#ifndef _TOPOLOGY_H_
#define _TOPOLOGY_H_

/* Genode includes */
#include <base/attached_rom_dataspace.h>
#include <os/reporter.h>

/* local includes */
#include <view/topology_dialog.h>

namespace Sculpt { struct Topology; }


struct Sculpt::Topology : Topology_dialog::Action
{
	Env &_env;

	Allocator &_alloc;

	Dialog::Generator &_dialog_generator;

	Slices _slices { };

	void _handle_topology()
	{
		_topology_rom.update();

		Slice_update_policy policy(_alloc);
		_slices.update_from_xml(policy, _topology_rom.xml());
		_dialog_generator.generate_dialog();
	}

	Attached_rom_dataspace _topology_rom {
		_env, "config -> topology" };

	Signal_handler<Topology> _topology_handler {
		_env.ep(), *this, &Topology::_handle_topology } ;

	Expanding_reporter _topology_filter { _env, "config", "topology_filter" };

	Topology_dialog dialog { _env, _dialog_generator, _slices };

	/**
	 * Topology_dialog::Action interface
	 */
	void slice_toggle(Slice::Name name) override
	{
		_topology_filter.generate([&] (Xml_generator &xml) {
			xml.node("rules", [&] () {
				_slices.for_each([&] (Slice &slice) {
					xml.node("match-element", [&] () {
						xml.attribute("name", "slice");
						xml.attribute("attribute", "name");
						xml.attribute("value", slice.name);
						xml.node("set-attribute", [&] () {
							xml.attribute("name", "enable");
							if ((slice.name == name) || (slice.parent == name)) {
								// toggle the enable state of the given slice
								slice.enable = !slice.enable;
							}
							xml.attribute("value", slice.enable);
						});
					});
				});
			});
		});
	}

    Topology(Env &env, Allocator &alloc, Dialog::Generator &dialog_generator)
		: _env(env), _alloc(alloc), _dialog_generator(dialog_generator)
	{
		_topology_rom.sigh(_topology_handler);
	}
};


#endif /* _TOPOLOGY_H_ */
