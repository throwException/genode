/*
 * \brief  Sculpt framebuffer management
 * \author Roman Iten
 * \date   2018-08-18
 */

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

/* Genode includes */
#include <base/attached_rom_dataspace.h>
#include <os/reporter.h>

/* local includes */
#include <model/discovery_state.h>
#include <model/fb_connectors.h>
#include <view/framebuffer_dialog.h>
#include <runtime.h>
#include <managed_config.h>

namespace Sculpt { struct Framebuffer; }


struct Sculpt::Framebuffer : Framebuffer_dialog::Action
{
	Env &_env;

	Allocator &_alloc;

	Dialog::Generator &_dialog_generator;

	Attached_rom_dataspace _fb_connectors_rom { _env, "report -> drivers/dynamic/intel_fb_drv/connectors" };

	Managed_config<Framebuffer> _fb_drv_config {
		_env, "config", "fb_drv", *this, &Framebuffer::_handle_fb_drv_config };

	void _handle_fb_drv_config(Xml_node)
	{
		//_fb_drv_config.try_generate_manually_managed();
	}

	Fb_connectors _fb_connectors { };

	Framebuffer_dialog dialog {
		_env, _dialog_generator, _fb_connectors };

	unsigned const _fallback_width  { 1920 };
	unsigned const _fallback_height { 1080 };

	void handle_fb_connectors_update();

	Signal_handler<Framebuffer> _fb_connector_update_handler {
		_env.ep(), *this, &Framebuffer::handle_fb_connectors_update };

	void _generate_fb_drv_config();

	Framebuffer(Env &env, Allocator &alloc, Dialog::Generator &dialog_generator)
	:
		_env(env), _alloc(alloc),
		_dialog_generator(dialog_generator)
	{
		_fb_connectors_rom    .sigh(_fb_connector_update_handler);
	}
};

#endif /* _STORAGE_H_ */
