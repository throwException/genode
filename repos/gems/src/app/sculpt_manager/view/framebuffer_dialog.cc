/*
 * \brief  Framebuffer management dialog
 * \author Roman Iten
 * \date   2018-08-18
 */

/* Genode includes */
#include <base/log.h>

/* local includes */
#include "framebuffer_dialog.h"


void Sculpt::Framebuffer_dialog::_gen_mode(Xml_generator           &xml,
                                           Fb_connector_mode const &mode,
                                           bool                     hz) const
{
	gen_named_node(xml, "hbox", mode.id(), [&] () {
		gen_named_node(xml, "float", "left", [&] () {
			xml.attribute("west", "yes");
			xml.node("hbox", [&] () {
				gen_named_node(xml, "button", "button", [&] () {

					if (_mode_item.hovered(mode.id()))
						xml.attribute("hovered", "yes");

					xml.node("label", [&] () { xml.attribute("text", " "); });
				});

				gen_named_node(xml, "label", "label", [&] () {
					xml.attribute("text", String<16>(" ", mode.width, "x", mode.height)); });
			});
		});

		if (hz) {
			gen_named_node(xml, "float", "right", [&] () {
				xml.attribute("east", "yes");
				xml.node("label", [&] () {
					xml.attribute("text", String<64>(mode.hz, " hz"));
				});
			});
    	}
	});
}


void Sculpt::Framebuffer_dialog::_gen_common_resolutions(Xml_generator &xml) const
{
	Fb_connector::Name connector_name { "Common Resolutions" };
	bool const selected = _connector_item.selected(connector_name);

	xml.node("button", [&] () {
		xml.attribute("name", connector_name);

		if (_connector_item.hovered(connector_name))
			xml.attribute("hovered", "yes");

		if (selected)
			xml.attribute("selected", "yes");

		xml.node("hbox", [&] () {
			gen_named_node(xml, "float", "info", [&] () {
				xml.attribute("west", "yes");

				xml.node("hbox", [&] () {

					gen_named_node(xml, "label", "device", [&] () {
						xml.attribute("text", connector_name); });
				});
			});
		});
	});

	if (selected) {
		xml.node("frame", [&] () {
			xml.attribute("name", connector_name);
			xml.node("vbox", [&] () {
				_fb_connectors.for_each([&] (Fb_connector const& connector) {
					connector.modes.for_each([&] (Fb_connector_mode const &mode) {
						if (mode.common(_fb_connectors)) {
							_gen_mode(xml, mode, true);
						}
					});
				});
			});
		});
	}
}


void Sculpt::Framebuffer_dialog::_gen_connector(Xml_generator      &xml,
                                                Fb_connector const &connector) const
{
	bool const selected = _connector_item.selected(connector.name);

	xml.node("button", [&] () {
		xml.attribute("name", connector.name);

		if (_connector_item.hovered(connector.name) && connector.connected)
			xml.attribute("hovered", "yes");

		if (selected && connector.connected)
			xml.attribute("selected", "yes");

		xml.node("hbox", [&] () {
			gen_named_node(xml, "float", "info", [&] () {
				xml.attribute("west", "yes");

				xml.node("hbox", [&] () {

					gen_named_node(xml, "label", "device", [&] () {
						xml.attribute("text", connector.name);
					});

				});
			});

			gen_named_node(xml, "float", "connected", [&] () {
				xml.attribute("east", "yes");
				xml.node("label", [&] () {
  					xml.attribute("text", connector.connected ? "connected" : "");
				});
			});
		});
	});

	if (selected) {
		xml.node("frame", [&] () {
			xml.attribute("name", connector.name);
			xml.node("vbox", [&] () {
				connector.modes.for_each([&] (Fb_connector_mode const &mode) {
					_gen_mode(xml, mode, true);
				});
			});
		});
	}
}


void Sculpt::Framebuffer_dialog::generate(Xml_generator &xml) const
{
	gen_named_node(xml, "frame", "framebuffer", [&] () {
		xml.node("vbox", [&] () {
			gen_named_node(xml, "label", "title", [&] () {
				xml.attribute("text", "Display");
				xml.attribute("font", "title/regular");
			});
			_gen_common_resolutions(xml);
			_fb_connectors.for_each([&] (Fb_connector const &connector) {
				_gen_connector(xml, connector);
			});
		});
	});
}


void Sculpt::Framebuffer_dialog::hover(Xml_node hover)
{
	bool const changed =
		_connector_item.match(hover, "vbox", "button", "name") |
		_mode_item     .match(hover, "vbox", "frame", "vbox", "hbox", "name");

	if (changed)
		_dialog_generator.generate_dialog();
}


void Sculpt::Framebuffer_dialog::click(Action &/* action */)
{
	_connector_item.toggle_selection_on_click();
	_mode_item.toggle_selection_on_click();

	_dialog_generator.generate_dialog();
}


void Sculpt::Framebuffer_dialog::clack(Action &/* action */)
{
	_dialog_generator.generate_dialog();
}

