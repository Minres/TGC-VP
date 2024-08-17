/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SRC_TGC_VP_TB_H_
#define SRC_TGC_VP_TB_H_

#include <generic/terminal.h>
#include <systemc>

#include "tgc_vp/rst_gen.h"
#include "tgc_vp/system.h"
namespace tgc_vp {

class tb : public sc_core::sc_module {
public:
    tb(sc_core::sc_module_name const& nm);
    tgc_vp::system top{"top"};
    tgc_vp::rst_gen rst_gen{"rst_gen"};
    sc_core::sc_signal<bool> rst_n{"rst_n"};
    sc_core::sc_vector<sc_core::sc_signal<bool>> pins_o{"pins_o", 32};
    sc_core::sc_vector<sc_core::sc_signal<bool>> pins_oe_o{"pins_oe_o", 32};
    sc_core::sc_vector<sc_core::sc_signal<bool>> pins_i{"pins_i", 32};
    sc_core::sc_signal<bool> uart0_tx_o{"uart0_tx_o"};
    sc_core::sc_signal<bool> uart0_rx_i{"uart0_rx_i"};
    sc_core::sc_vector<sc_core::sc_signal<bool>> t0_clear_i{"t0_clear_i", vpvper::minres::timer::CLEAR_CNT};
    sc_core::sc_vector<sc_core::sc_signal<bool>> t0_tick_i{"t0_tick_i", vpvper::minres::timer::TICK_CNT - 1};
    sc_core::sc_signal<bool> ssclk_o{"ssclk_o"};
    sc_core::sc_vector<sc_core::sc_signal<bool>> dq_o{"dq_o", 4};
    sc_core::sc_vector<sc_core::sc_signal<bool>> dq_oe_o{"dq_oe_o", 4};
    sc_core::sc_vector<sc_core::sc_signal<bool>> dq_i{"dq_i", 4};
    sc_core::sc_signal<sc_core::sc_time> clk_i{"clk_i"};
};

} /* namespace tgc_vp */

#endif /* SRC_TGC_VP_TB_H_ */
