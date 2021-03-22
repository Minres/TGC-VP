/*******************************************************************************
 * Copyright (C) 2017, 2018 MINRES Technologies GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include <tgfs_vp/system.h>

namespace tgfs_vp {
using namespace sc_core;
using namespace vpvper::sifive;
using namespace sysc::tgfs;

system::system(sc_core::sc_module_name nm)
: sc_core::sc_module(nm)
, NAMED(router,  platfrom_mmap.size() + 2, 1)
, NAMEDC(qspi0_ptr, spi, spi_impl::beh)
, NAMEDC(qspi1_ptr, spi, spi_impl::beh)
, NAMEDC(qspi2_ptr, spi, spi_impl::beh)
, qspi0(*qspi0_ptr)
, qspi1(*qspi1_ptr)
, qspi2(*qspi2_ptr)
{
    auto& qspi0 = *qspi0_ptr;
    auto& qspi1 = *qspi1_ptr;
    auto& qspi2 = *qspi2_ptr;
    core_complex.initiator(router.target[0]);
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

    uart0.clk_i(tlclk_s);
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
    core_complex.global_irq_i(core_int_s);
    core_complex.local_irq_i(local_int_s);

    pins_i(gpio0.pins_i);
    gpio0.pins_o(pins_o);

    gpio0.iof0_i[17](uart0.tx_o);
    uart0.rx_i(gpio0.iof0_o[16]);
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

    s_dummy_sck_i[0](uart1.tx_o);
    uart1.rx_i(s_dummy_sck_o[0]);
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

    for (auto &sock : s_dummy_sck_i) sock.error_if_no_callback = false;
}

} /* namespace sysc */
