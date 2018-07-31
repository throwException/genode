/*
 * \brief  Topology management dialog
 * \author Roman Iten
 * \date   2018-07-30
 */

/*
 * Copyright (C) 2018 gapfruit AG
 */

#ifndef _VIEW__TOPOLOGY_DIALOG_H_
#define _VIEW__TOPOLOGY_DIALOG_H_

#include <types.h>
#include <model/slice.h>
#include <model/nic_target.h>
#include <model/nic_state.h>
#include <model/wifi_connection.h>
#include <model/wpa_passphrase.h>
#include <model/pci_info.h>
#include <view/dialog.h>
#include <view/selectable_item.h>

namespace Sculpt { struct Topology_dialog; }


struct Sculpt::Topology_dialog : Dialog
{
	Env &_env;

	Dialog::Generator &_dialog_generator;

	Slices const &_slices;

	Hoverable_item  _slice_item { };

	Slice::Name selected_slice() const { return _slice_item._hovered; }

	void generate(Xml_generator &) const;

	/**
	 * Dialog interface
	 */
	void hover(Xml_node hover) override;

	struct Action : Interface
	{
		virtual void slice_toggle(Slice::Name) = 0;
	};

	void click(Action &action);

	Topology_dialog(Env                     &env,
	                Dialog::Generator       &dialog_generator,
	                Slices            const &slices)
	:
	    _env(env), _dialog_generator(dialog_generator), _slices(slices)
	{ }
};

#endif /* _VIEW__TOPOLOGY_DIALOG_H_ */
