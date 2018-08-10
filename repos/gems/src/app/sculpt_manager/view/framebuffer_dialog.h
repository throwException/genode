/*
 * \brief  Framebuffer management dialog
 * \author Roman Iten
 * \date   2018-08-18
 */

#ifndef _VIEW__FRAMEBUFFER_DIALOG_H_
#define _VIEW__FRAMEBUFFER_DIALOG_H_

#include <types.h>
#include <model/storage_devices.h>
#include <model/storage_target.h>
#include <model/fb_connectors.h>
#include <model/ram_fs_state.h>
#include <view/selectable_item.h>
#include <view/activatable_item.h>
#include <view/dialog.h>

namespace Sculpt { struct Framebuffer_dialog; }


struct Sculpt::Framebuffer_dialog : Dialog
{
	Env &_env;

	Dialog::Generator &_dialog_generator;

	Fb_connectors const &_fb_connectors;

	Selectable_item  _connector_item { };
	Selectable_item  _mode_item      { };

	void generate(Xml_generator &) const;

	void _gen_common_resolutions(Xml_generator &xml) const;

	void _gen_connector(Xml_generator &, Fb_connector const &) const;

	void _gen_mode(Xml_generator &, /* Fb_connector const &, */ Fb_connector_mode const &, bool) const;

	void hover(Xml_node hover) override;

	struct Action : Interface
	{
		// Placeholder the define the actions of the interactive
		// framebuffer dialog.
	};

	Fb_connector::Name _selected_connector() const
	{
		return _connector_item._selected;
	}

	Fb_connector_mode::Id _selected_mode() const
	{
		return _mode_item._selected;
	}

	void click(Action &action);

	void clack(Action &action);

	Framebuffer_dialog(Env &env, Dialog::Generator &dialog_generator,
	                   Fb_connectors const &fb_connectors)
	:
		_env(env), _dialog_generator(dialog_generator),
		_fb_connectors(fb_connectors)
	{ }
};

#endif /* _FRAMEBUFFER_DIALOG_H_ */
