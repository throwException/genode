/*
 * \brief   i.MX6Quad Apalis specific board definitions
 * \author  Stefan Kalkowski
 * \author  Pirmin Duss
 * \date    2019-05-16
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__INCLUDE__HW__SPEC__ARM__IMX6Q_APALIS_BOARD_H_
#define _SRC__INCLUDE__HW__SPEC__ARM__IMX6Q_APALIS_BOARD_H_

#include <drivers/defs/imx6q_apalis.h>
#include <drivers/uart/imx.h>
#include <hw/spec/arm/boot_info.h>
#include <hw/spec/arm/cortex_a9.h>
#include <hw/spec/arm/pl310.h>

namespace Hw::Imx6q_apalis_board {

	using namespace Imx6q_apalis;

	using Cpu_mmio = Hw::Cortex_a9_mmio<CORTEX_A9_PRIVATE_MEM_BASE>;
	using Serial   = Genode::Imx_uart;

	enum {
		UART_BASE  = UART_1_MMIO_BASE,
		UART_SIZE  = UART_1_MMIO_SIZE,
		UART_CLOCK = 0, /* dummy value, not used */
	};
}

#endif /* _SRC__INCLUDE__HW__SPEC__ARM__IMX6Q_APALIS_BOARD_H_ */
