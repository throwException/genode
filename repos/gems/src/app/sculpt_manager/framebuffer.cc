/*
 * \brief  Sculpt framebuffer management
 * \author Roman Iten
 * \date   2018-08-18
 */

/* local includes */
#include <framebuffer.h>
#include <model/fb_connectors.h>


void Sculpt::Framebuffer::handle_fb_connectors_update()
{
	_fb_connectors_rom.update();

	Fb_connector_update_policy policy(_alloc, _fb_connectors);
	_fb_connectors.update_from_xml(policy, _fb_connectors_rom.xml());

	_generate_fb_drv_config();

	_dialog_generator.generate_dialog();
}


void Sculpt::Framebuffer::_generate_fb_drv_config()
{
	unsigned max_width = 0;
	unsigned max_height = 0;

	_fb_connectors.for_each([&] (Fb_connector const& connector) {
		if (connector.connected) {
			connector.modes.for_each([&] (Fb_connector_mode const& mode) {
				if (mode.common(_fb_connectors) && ((mode.width*mode.height) > (max_width*max_height))) {
					max_width = mode.width;
					max_height = mode.height;
				}
			});
		}
	});

	if ((max_width == 0) || (max_height == 0)) {
		warning("no common resolution found, using fallback (", _fallback_width, "x", _fallback_height, ")");
		max_width = _fallback_width;
		max_height = _fallback_height;
	}

	_fb_drv_config.generate([&] (Xml_generator &xml) {
		xml.attribute("buffered", "yes");

		xml.node("report", [&] () {
			xml.attribute("connectors", "yes");
		});

		_fb_connectors.for_each([&] (Fb_connector const& connector) {
			bool enabled = false;
			connector.modes.for_each([&] (Fb_connector_mode const& mode) {
				if (!enabled && (mode.width  == max_width) && (mode.height == max_height)) {
					xml.node("connector", [&] () {
						xml.attribute("name", connector.name);
						xml.attribute("width", mode.width);
						xml.attribute("height", mode.height);
						xml.attribute("hz", mode.hz);
						xml.attribute("enabled", true);
					});
					enabled = true;
				}
			});
		});
	});
}
