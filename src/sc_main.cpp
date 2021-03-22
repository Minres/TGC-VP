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

#include "CLIParser.h"
#include <iss/log_categories.h>

#include <scc/configurable_tracer.h>
#include <scc/configurer.h>
#include <scc/report.h>
#include <scc/scv_tr_db.h>
#include <scc/tracer.h>
#include <scc/perf_estimator.h>

#include <boost/program_options.hpp>
#include <tgfs_vp/tb.h>
#include <fstream>
#include <sstream>
const std::string core_path{"tb.top.core_complex"};

using namespace sysc;
using namespace sc_core;
namespace po = boost::program_options;

namespace {
const size_t ERROR_IN_COMMAND_LINE = 1;
const size_t SUCCESS = 0;
const size_t ERROR_UNHANDLED_EXCEPTION = 2;
} // namespace

int sc_main(int argc, char *argv[]) {
    ///////////////////////////////////////////////////////////////////////////
    // SystemC >=2.2 got picky about multiple drivers so disable check
    ///////////////////////////////////////////////////////////////////////////
    sc_report_handler::set_actions(SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, SC_DO_NOTHING);
    ///////////////////////////////////////////////////////////////////////////
    // CLI argument parsing & logging setup
    ///////////////////////////////////////////////////////////////////////////
    CLIParser parser(argc, argv);
    if (!parser.is_valid()) return ERROR_IN_COMMAND_LINE;
    ///////////////////////////////////////////////////////////////////////////
    // create the performance estimation module
    ///////////////////////////////////////////////////////////////////////////
    scc::perf_estimator estimator;
  ///////////////////////////////////////////////////////////////////////////
    // set up configuration
    ///////////////////////////////////////////////////////////////////////////
    scc::configurer cfg(parser.get<std::string>("config-file"));
    ///////////////////////////////////////////////////////////////////////////
    // set up tracing & transaction recording
    ///////////////////////////////////////////////////////////////////////////
    auto trace_level = parser.get<unsigned>("trace-level");
    scc::configurable_tracer trace(parser.get<std::string>("trace-file"),
            static_cast<scc::tracer::file_type>(trace_level >> 1), // bit3-bit1 define the kind of transaction trace
            (trace_level&0x1) != 0, // bit0 enables vcd
            parser.is_set("trace-default-on"));
    ///////////////////////////////////////////////////////////////////////////
    // instantiate top level
    ///////////////////////////////////////////////////////////////////////////
    auto i_system = scc::make_unique<tgfs_vp::tb>("tb");
    ///////////////////////////////////////////////////////////////////////////
    // add non-implemented 'enableTracing' properties
    ///////////////////////////////////////////////////////////////////////////
    trace.add_control();
    ///////////////////////////////////////////////////////////////////////////
    // dump configuration if requested
    ///////////////////////////////////////////////////////////////////////////
    if (parser.get<std::string>("dump-config").size() > 0) {
        std::ofstream of{parser.get<std::string>("dump-config")};
        if (of.is_open()) cfg.dump_configuration(of);
    }
    cfg.configure();
    ///////////////////////////////////////////////////////////////////////////
    // overwrite config with command line settings
    ///////////////////////////////////////////////////////////////////////////
    cfg.set_value(core_path + ".gdb_server_port", parser.get<unsigned short>("gdb-port"));
    cfg.set_value(core_path + ".dump_ir", parser.is_set("dump-ir"));
    if (parser.is_set("elf")) cfg.set_value(core_path + ".elf_file", parser.get<std::string>("elf"));
    if (parser.is_set("quantum"))
        tlm::tlm_global_quantum::instance().set(sc_core::sc_time(parser.get<unsigned>("quantum"), sc_core::SC_NS));
    if (parser.is_set("reset")) {
        auto str = parser.get<std::string>("reset");
        uint64_t start_address = str.find("0x") == 0 ? std::stoull(str.substr(2), nullptr, 16) : std::stoull(str, nullptr, 10);
        cfg.set_value(core_path + ".reset_address", start_address);
    }
    if (parser.is_set("disass")) {
        cfg.set_value(core_path + ".enable_disass", true);
        LOGGER(disass)::reporting_level() = logging::INFO;
        auto file_name = parser.get<std::string>("disass");
        if (file_name.length() > 0) {
            LOG_OUTPUT(disass)::stream() = fopen(file_name.c_str(), "w");
            LOGGER(disass)::print_time() = false;
            LOGGER(disass)::print_severity() = false;
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    // instantiate perf estimator to get some simulation performance figures
    ///////////////////////////////////////////////////////////////////////////
    scc::perf_estimator pt;
    ///////////////////////////////////////////////////////////////////////////
    // run simulation
    ///////////////////////////////////////////////////////////////////////////
    try {
        if (parser.is_set("max_time")) {
            sc_core::sc_start(scc::parse_from_string(parser.get<std::string>("max_time")));
        } else
            sc_core::sc_start();
        if (!sc_core::sc_end_of_simulation_invoked()) sc_core::sc_stop();
    } catch (sc_core::sc_report &rep) {
        sc_core::sc_report_handler::get_handler()(rep, sc_core::SC_DISPLAY | sc_core::SC_STOP);
    }
    return 0;
}
