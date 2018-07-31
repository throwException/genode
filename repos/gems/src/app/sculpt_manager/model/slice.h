/*
 * \brief  Representation of a slice
 * \author Roman Iten
 * \date   2018-07-31
 */

/*
 * Copyright (C) 2018 gapfruit AG
 */

#ifndef _MODEL__SLICE_H_
#define _MODEL__SLICE_H_

#include "types.h"

namespace Sculpt {

	struct Slice;
	struct Slice_update_policy;

	typedef List_model<Slice> Slices;
};


struct Sculpt::Slice : List_model<Slice>::Element
{
	typedef String<32> Name;

	Name const name;
	bool       enable;

	Slice(Name const &name, bool enable)
	: name(name), enable(enable) { }
};


/**
 * Policy for transforming a 'topology' report into a list model
 */
struct Sculpt::Slice_update_policy : List_model<Slice>::Update_policy
{
	Allocator &_alloc;

	Slice_update_policy(Allocator &alloc) : _alloc(alloc) { }

	void destroy_element(Slice &elem) { destroy(_alloc, &elem); }

	Slice &create_element(Xml_node const& node)
	{
		return *new (_alloc)
			Slice(node.attribute_value("name", Slice::Name()),
			      node.attribute_value("enable", true));
	}

	void update_element(Slice& /*slice*/, const Xml_node& /*node*/)
	{
		// Do not update the enable status, as the
		// sculpt_manager GUI takes care of it.
	}

	static bool element_matches_xml_node(Slice const& elem, Xml_node const& node)
	{
		return node.attribute_value("name", Slice::Name()) == elem.name;
	}
};

#endif /* _MODEL__SLICE_H_ */
