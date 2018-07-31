/*
 * \brief  Topology management dialog
 * \author Roman Iten
 * \date   2018-07-30
 */

/*
 * Copyright (C) 2018 gapfruit AG
 */

/* Genode includes */
#include <base/log.h>

/* local includes */
#include "topology_dialog.h"


void Sculpt::Topology_dialog::generate(Xml_generator &xml) const
{
	gen_named_node(xml, "frame", "topology", [&] () {
		xml.node("vbox", [&] () {
			gen_named_node(xml, "label", "title", [&] () {
				xml.attribute("text", "Topology");
				xml.attribute("font", "title/regular");
			});

			gen_named_node(xml, "vbox", "type", [&] () {

				auto gen_slice_button = [&] (Slice::Name const &id,
				                             bool        const  enable) {
					gen_named_node(xml, "button", id, [&] () {

						_slice_item.gen_button_attr(xml, id);

						xml.attribute("selected", enable);
						xml.node("label", [&] () { xml.attribute("text", id); });
					});
				};

				_slices.for_each([&] (Slice const &slice) {
					if (slice.parent == "") {
						gen_slice_button(slice.name, slice.enable); 
					}
				});
			});
		});
	});
}


void Sculpt::Topology_dialog::hover(Xml_node hover)
{
	_slice_item.match(hover, "vbox", "vbox", "button", "name");

	_dialog_generator.generate_dialog();
}


void Sculpt::Topology_dialog::click(Action &action)
{
	action.slice_toggle(selected_slice());

	_dialog_generator.generate_dialog();
}
