/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SRC_TGFS_VP_TB_H_
#define SRC_TGFS_VP_TB_H_

#include <tgfs_vp/system.h>
#include <generic/terminal.h>

#include <systemc>
namespace tgfs_vp {

class tb : public sc_core::sc_module {
public:
    tb(sc_core::sc_module_name const& nm);
    tgfs_vp::system top{"top"};
    sc_core::sc_vector<tlm::scc::tlm_signal<sc_dt::sc_logic>> gpio_s{"gpio_s", 32};
    sc_core::sc_signal<bool> rst_n{"rst_n"};
    vpvper::generic::terminal terminal{"terminal"};
private:
    void run();
};

} /* namespace tgfs_vp */

#endif /* SRC_TGFS_VP_TB_H_ */
