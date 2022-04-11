/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <sifive/aon.h>
#include <sifive/clint.h>
#include <sifive/gpio.h>
#include <sifive/plic.h>
#include <sifive/prci.h>
#include <sifive/pwm.h>
#include <sifive/spi.h>
#include <sysc/core_complex.h>
#include <sifive/uart.h>
#include <sifive/uart_terminal.h>
#include <cci_configuration>
#include <scc/memory.h>
#include <scc/router.h>
#include <scc/utilities.h>
#include <tlm/scc/tlm_signal_sockets.h>
#include <array>
#include <memory>
#include <sysc/kernel/sc_module.h>

namespace tgc_vp {

class system : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(system);// NOLINT

    sc_core::sc_vector<tlm::scc::tlm_signal_initiator_socket<sc_dt::sc_logic>> pins_o{"pins_o", 32};
    sc_core::sc_vector<tlm::scc::tlm_signal_target_socket<sc_dt::sc_logic>> pins_i{"pins_i", 32};

    sc_core::sc_in<bool> erst_n{"erst_n"};

    system(sc_core::sc_module_name nm);

private:
    sysc::tgfs::core_complex core_complex{"core_complex"};
    scc::router<> router;
    vpvper::sifive::uart_terminal uart0{"uart0"};
    vpvper::sifive::uart  uart1{"uart1"};
    std::unique_ptr<vpvper::sifive::spi> qspi0_ptr, qspi1_ptr, qspi2_ptr;
    vpvper::sifive::pwm pwm0{"pwm0"}, pwm1{"pwm1"}, pwm2{"pwm2"};
    vpvper::sifive::gpio gpio0{"gpio0"};
    vpvper::sifive::plic plic{"plic"};
    vpvper::sifive::aon aon{"aon"};
    vpvper::sifive::prci prci{"prci"};
    vpvper::sifive::clint clint{"clint"};

    using mem_qspi_t = scc::memory<512_MB, 32>;
    mem_qspi_t mem_qspi{"mem_qspi"};
    using mem_ram_t = scc::memory<128_kB, 32>;
    mem_ram_t mem_ram{"mem_ram"};

    sc_core::sc_signal<sc_core::sc_time, sc_core::SC_MANY_WRITERS> tlclk_s{"tlclk_s"};
    sc_core::sc_signal<sc_core::sc_time, sc_core::SC_MANY_WRITERS> lfclk_s{"lfclk_s"};
    
    sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> rst_s{"rst_s"}, mtime_int_s{"mtime_int_s"}, msie_int_s{"msie_int_s"};
    
    sc_core::sc_vector<sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>> global_int_s{"global_int_s", 256}, local_int_s{"local_int_s", 16};
    sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> core_int_s{"core_int_s"};
    
    sc_core::sc_vector<tlm::scc::tlm_signal_bool_opt_in> s_dummy_sck_i{"s_dummy_sck_i", 16};
    sc_core::sc_vector<tlm::scc::tlm_signal_bool_opt_out> s_dummy_sck_o{"s_dummy_sck_o", 16};

protected:
    void gen_reset();
    vpvper::sifive::spi& qspi0;
    vpvper::sifive::spi& qspi1;
    vpvper::sifive::spi& qspi2;
#include "tgc_vp/gen/platform_mmap.h"
};

} /* namespace sysc */

#endif /* _PLATFORM_H_ */
