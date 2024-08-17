/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "minres/irq.h"
#include "minres/timer.h"
#include <array>
#include <cci_configuration>
#include <memory>
#include <minres/aclint.h>
#include <minres/gpio.h>
#include <minres/qspi.h>
#include <minres/uart.h>
#include <scc/memory.h>
#include <scc/router.h>
#include <scc/utilities.h>
#include <sysc/communication/sc_clock.h>
#include <sysc/communication/sc_signal_ports.h>
#include <sysc/core_complex.h>
#include <sysc/kernel/sc_module.h>
#include <sysc/kernel/sc_time.h>
#include <sysc/utils/sc_vector.h>
#include <tlm/scc/tlm_signal_sockets.h>

namespace tgc_vp {

class system : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(system); // NOLINT

    sc_core::sc_vector<sc_core::sc_out<bool>> pins_o{"pins_o", 32};
    sc_core::sc_vector<sc_core::sc_out<bool>> pins_oe_o{"pins_oe_o", 32};
    sc_core::sc_vector<sc_core::sc_in<bool>> pins_i{"pins_i", 32};
    sc_core::sc_out<bool> uart0_tx_o{"uart0_tx_o"};
    sc_core::sc_in<bool> uart0_rx_i{"uart0_rx_i"};
    sc_core::sc_vector<sc_core::sc_in<bool>> t0_clear_i{"t0_clear_i", vpvper::minres::timer::CLEAR_CNT};
    sc_core::sc_vector<sc_core::sc_in<bool>> t0_tick_i{"t0_tick_i", vpvper::minres::timer::TICK_CNT - 1};
    sc_core::sc_out<bool> ssclk_o{"ssclk_o"};
    sc_core::sc_vector<sc_core::sc_out<bool>> dq_o{"dq_o", 4};
    sc_core::sc_vector<sc_core::sc_out<bool>> dq_oe_o{"dq_oe_o", 4};
    sc_core::sc_vector<sc_core::sc_in<bool>> dq_i{"dq_i", 4};

    sc_core::sc_in<sc_core::sc_time> clk_i{"clk_i"};

    sc_core::sc_in<bool> erst_n{"erst_n"};

    system(sc_core::sc_module_name nm);

private:
    sysc::tgfs::core_complex<> core_complex{"core_complex"};
    scc::router<> ahb_router, apbBridge;
    vpvper::minres::gpio_tl gpio0{"gpio0"};
    vpvper::minres::uart_tl uart0{"uart0"};
    vpvper::minres::timer_tl timer0{"timer0"};
    vpvper::minres::aclint_tl aclint{"aclint"};
    vpvper::minres::irq_tl irq_ctrl{"irq_ctrl"};
    vpvper::minres::qspi_tl qspi{"qspi"};

    scc::memory<1_kB, scc::LT> boot_rom{"boot_rom"};
    scc::memory<32_kB, scc::LT> main_ram{"main_ram"};

    sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> rst_s{"rst_s"}, mtime_int_s{"mtime_int_s"},
        msip_int_s{"msip_int_s"};

    sc_core::sc_vector<sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>> irq_int_s{"irq_int_s", 32},
        local_int_s{"local_int_s", 16};
    sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> core_int_s{"core_int_s"};

    void gen_reset();
#include "tgc_vp/gen/PipelinedMemoryBusToApbBridge.h"
};

} // namespace tgc_vp

#endif /* _PLATFORM_H_ */
