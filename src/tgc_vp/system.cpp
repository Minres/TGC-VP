/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tgc_vp/system.h"
#include "minres/timer.h"
#include "minres/uart.h"
#include "scc/utilities.h"

namespace tgc_vp {
using namespace sc_core;
using namespace vpvper::minres;
using namespace sysc::tgfs;

system::system(sc_core::sc_module_name nm)
: sc_core::sc_module(nm)
, NAMED(ahb_router, 3, 2)
, NAMED(apbBridge, PipelinedMemoryBusToApbBridge_map.size(), 1){
    core_complex.ibus(ahb_router.target[0]);
    core_complex.dbus(ahb_router.target[1]);

    ahb_router.initiator.at(0)(qspi.xip_sck);
    ahb_router.set_target_range(0, 0xE0000000, 16_MB);
    ahb_router.initiator.at(1)(main_ram.target);
    ahb_router.set_target_range(1, 0x80000000, 32_kB);
    ahb_router.initiator.at(2)(apbBridge.target[0]);
    ahb_router.set_target_range(2, 0xF0000000, 256_MB);

    size_t i = 0;
    for (const auto &e : PipelinedMemoryBusToApbBridge_map) {
        apbBridge.initiator.at(i)(e.target);
        apbBridge.set_target_range(i, e.start, e.size);
        i++;
    }

    gpio0.clk_i(clk_i);
    uart0.clk_i(clk_i);
    timer0.clk_i(clk_i);
    aclint.clk_i(clk_i);
    irq_ctrl.clk_i(clk_i);
    qspi.clk_i(clk_i);
    boot_rom.clk_i(clk_i);
    core_complex.clk_i(clk_i);
    main_ram.clk_i(clk_i);

    gpio0.rst_i(rst_s);
    uart0.rst_i(rst_s);
    timer0.rst_i(rst_s);
    aclint.rst_i(rst_s);
    irq_ctrl.rst_i(rst_s);
    qspi.rst_i(rst_s);
    core_complex.rst_i(rst_s);

    aclint.mtime_int_o(mtime_int_s);
    aclint.msip_int_o(msip_int_s);
    irq_ctrl.irq_o(core_int_s);
    irq_ctrl.pending_irq_i(irq_int_s);

    uart0.irq_o(irq_int_s[0]);
    timer0.interrupt_o[0](irq_int_s[1]);
    timer0.interrupt_o[1](irq_int_s[2]);
    qspi.irq_o(irq_int_s[3]);

    core_complex.timer_irq_i(mtime_int_s);
    core_complex.ext_irq_i(core_int_s);
    core_complex.local_irq_i(local_int_s);
    core_complex.sw_irq_i(msip_int_s);

    gpio0.pins_i(pins_i);
    gpio0.pins_o(pins_o);
    gpio0.oe_o(pins_oe_o);

    uart0.tx_o(uart0_tx_o);
    uart0.rx_i(uart0_rx_i);

    timer0.clear_i(t0_clear_i);
    timer0.tick_i(t0_tick_i);

    qspi.ssclk_o(ssclk_o);
    qspi.dq_o(dq_o);
    qspi.dq_i(dq_i);
    qspi.oe_o(dq_oe_o);

    SC_METHOD(gen_reset);
    sensitive << erst_n;
}
void system::gen_reset(){
    rst_s = !erst_n.read();
}


} /* namespace sysc */
