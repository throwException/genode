/*
 * \brief  MMIO and IRQ definitions of the i.MX6Quad Apalis
 * \author Stefan Kalkowski
 * \author Pirmin Duss
 * \date   2019-01-25
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 * Copyright (C) 2019 gapfruit AG
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__DRIVERS__DEFS__IMX6Q_APALIS_H_
#define _INCLUDE__DRIVERS__DEFS__IMX6Q_APALIS_H_

/* Genode includes */
#include <drivers/defs/imx6.h>

namespace Imx6q_apalis {

	using namespace Imx6;

	enum {
		/* normal RAM */
		RAM_BASE = 0x10000000,
		RAM_SIZE = 0x40000000,
	};
};

#endif /* _INCLUDE__DRIVERS__DEFS__IMX6Q_APALIS_H_ */
