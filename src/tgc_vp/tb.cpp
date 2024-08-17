/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tgc_vp/tb.h"
#include <sysc/kernel/sc_time.h>
namespace tgc_vp {

SC_HAS_PROCESS(tb);
tb::tb(const sc_core::sc_module_name& nm)
: sc_core::sc_module(nm) {
    top.erst_n(rst_n);
    rst_gen.rst_n(rst_n);
    top.pins_o(pins_o);
    top.pins_i(pins_i);
    top.pins_oe_o(pins_oe_o);
    top.uart0_rx_i(uart0_rx_i);
    top.uart0_tx_o(uart0_tx_o);
    top.t0_clear_i(t0_clear_i);
    top.t0_tick_i(t0_tick_i);
    top.ssclk_o(ssclk_o);
    top.dq_o(dq_o);
    top.dq_i(dq_i);
    top.dq_oe_o(dq_oe_o);
    top.clk_i(clk_i);
    clk_i = 10_ns;
}
} // namespace tgc_vp
