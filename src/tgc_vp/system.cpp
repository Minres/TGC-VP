/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tgc_vp/system.h"

namespace tgc_vp {
using namespace sc_core;
using namespace vpvper::sifive;
using namespace sysc::tgfs;

system::system(sc_core::sc_module_name nm)
: sc_core::sc_module(nm)
, NAMED(router,  platfrom_mmap.size() + 2, 2)
{
    core_complex.ibus(router.target[0]);
    core_complex.dbus(router.target[1]);
    size_t i = 0;
    for (const auto &e : platfrom_mmap) {
        router.initiator.at(i)(e.target);
        router.set_target_range(i, e.start, e.size);
        i++;
    }
    router.initiator.at(i)(mem_qspi.target);
    router.set_target_range(i, 0x20000000, 512_MB);
    router.initiator.at(++i)(mem_ram.target);
    router.set_target_range(i, 0x80000000, 128_kB);

    uart1.clk_i(tlclk_s);
    qspi0.clk_i(tlclk_s);
    qspi1.clk_i(tlclk_s);
    qspi2.clk_i(tlclk_s);
    pwm0.clk_i(tlclk_s);
    pwm1.clk_i(tlclk_s);
    pwm2.clk_i(tlclk_s);
    gpio0.clk_i(tlclk_s);
    plic.clk_i(tlclk_s);
    aon.clk_i(tlclk_s);
    aon.lfclkc_o(lfclk_s);
    prci.hfclk_o(tlclk_s); // clock driver
    clint.tlclk_i(tlclk_s);
    clint.lfclk_i(lfclk_s);
    core_complex.clk_i(tlclk_s);
    mem_qspi.clk_i(tlclk_s);
    mem_ram.clk_i(tlclk_s);

    uart0.rst_i(rst_s);
    uart1.rst_i(rst_s);
    qspi0.rst_i(rst_s);
    qspi1.rst_i(rst_s);
    qspi2.rst_i(rst_s);
    pwm0.rst_i(rst_s);
    pwm1.rst_i(rst_s);
    pwm2.rst_i(rst_s);
    gpio0.rst_i(rst_s);
    plic.rst_i(rst_s);
    aon.rst_o(rst_s);
    prci.rst_i(rst_s);
    clint.rst_i(rst_s);
    core_complex.rst_i(rst_s);

    aon.erst_n_i(erst_n);

    clint.mtime_int_o(mtime_int_s);
    clint.msip_int_o(msie_int_s);

    plic.global_interrupts_i(global_int_s);
    plic.core_interrupt_o(core_int_s);

    core_complex.sw_irq_i(msie_int_s);
    core_complex.timer_irq_i(mtime_int_s);
    core_complex.ext_irq_i(core_int_s);
    core_complex.local_irq_i(local_int_s);

    pins_i(gpio0.pins_i);
    gpio0.pins_o(pins_o);

    uart0.irq_o(global_int_s[3]);

    gpio0.iof0_i[5](qspi1.sck_o);
    gpio0.iof0_i[3](qspi1.mosi_o);
    qspi1.miso_i(gpio0.iof0_o[4]);
    gpio0.iof0_i[2](qspi1.scs_o[0]);
    gpio0.iof0_i[9](qspi1.scs_o[2]);
    gpio0.iof0_i[10](qspi1.scs_o[3]);

    qspi0.irq_o(global_int_s[5]);
    qspi1.irq_o(global_int_s[6]);
    qspi2.irq_o(global_int_s[7]);

    gpio0.iof0_i[16](uart1.tx_o);
    uart1.rx_i(gpio0.iof0_o[17]);
    uart1.irq_o(global_int_s[4]);

    gpio0.iof1_i[0](pwm0.cmpgpio_o[0]);
    gpio0.iof1_i[1](pwm0.cmpgpio_o[1]);
    gpio0.iof1_i[2](pwm0.cmpgpio_o[2]);
    gpio0.iof1_i[3](pwm0.cmpgpio_o[3]);

    gpio0.iof1_i[10](pwm2.cmpgpio_o[0]);
    gpio0.iof1_i[11](pwm2.cmpgpio_o[1]);
    gpio0.iof1_i[12](pwm2.cmpgpio_o[2]);
    gpio0.iof1_i[13](pwm2.cmpgpio_o[3]);

    gpio0.iof1_i[19](pwm1.cmpgpio_o[0]);
    gpio0.iof1_i[20](pwm1.cmpgpio_o[1]);
    gpio0.iof1_i[21](pwm1.cmpgpio_o[2]);
    gpio0.iof1_i[22](pwm1.cmpgpio_o[3]);

    pwm0.cmpip_o[0](global_int_s[40]);
    pwm0.cmpip_o[1](global_int_s[41]);
    pwm0.cmpip_o[2](global_int_s[42]);
    pwm0.cmpip_o[3](global_int_s[43]);

    pwm1.cmpip_o[0](global_int_s[44]);
    pwm1.cmpip_o[1](global_int_s[45]);
    pwm1.cmpip_o[2](global_int_s[46]);
    pwm1.cmpip_o[3](global_int_s[47]);

    pwm2.cmpip_o[0](global_int_s[48]);
    pwm2.cmpip_o[1](global_int_s[49]);
    pwm2.cmpip_o[2](global_int_s[50]);
    pwm2.cmpip_o[3](global_int_s[51]);
}

} /* namespace sysc */
