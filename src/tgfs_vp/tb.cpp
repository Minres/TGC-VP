#include "tb.h"
namespace tgfs_vp {

SC_HAS_PROCESS(tb);
tb::tb(const sc_core::sc_module_name &nm): sc_core::sc_module(nm) {
    top.erst_n(rst_n);
    for (auto i = 0U; i < gpio_s.size(); ++i) {
        gpio_s[i].in(top.pins_o[i]);
        top.pins_i[i](gpio_s[i].out);
    }
    SC_THREAD(run);
    // terminal
    terminal.tx_o(gpio_s[16].in);
    gpio_s[17].out(terminal.rx_i);
}

void tb::run() {
    rst_n.write(false);
    wait(100_ns);
    rst_n.write(true);
}

}
