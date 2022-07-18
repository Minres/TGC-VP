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
    sc_core::sc_vector<tlm::scc::tlm_signal<sc_dt::sc_logic>> gpio_s{"gpio_s", 32};
    sc_core::sc_signal<bool> rst_n{"rst_n"};
#ifndef WIN32 // Seasocks not available under windows
    vpvper::generic::terminal terminal{"terminal"};
#endif
};

} /* namespace tgc_vp */

#endif /* SRC_TGC_VP_TB_H_ */
