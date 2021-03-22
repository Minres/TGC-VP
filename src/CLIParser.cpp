/*******************************************************************************
 * Copyright (C) 2018 MINRES Technologies GmbH
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
#include <scc/report.h>
#include <iostream>
#include <iss/log_categories.h>
#include <scc/report.h>
#include <stdexcept>

namespace po = boost::program_options;
using namespace sc_core;

CLIParser::CLIParser(int argc, char *argv[])
: desc("Options")
, valid(false) {
    build();
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm_); // can throw
        // --help option
        if (vm_.count("help")) {
            std::cout << "DBT-RISE-RiscV simulator for RISC-V" << std::endl << desc << std::endl;
        }
        po::notify(vm_); // throws on error, so do after help in case there are any problems
        valid = true;
    } catch (po::error &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
    }
    auto log_level = vm_["verbose"].as<scc::log>();
    auto log_level_num = static_cast<unsigned>(log_level);
    auto l = logging::as_log_level(log_level_num > 6 ? 6 : log_level_num);
    LOGGER(DEFAULT)::reporting_level() = l;
    LOGGER(DEFAULT)::print_time() = false;
    LOGGER(connection)::reporting_level() = l;
    LOGGER(connection)::print_time() = false;
    ///////////////////////////////////////////////////////////////////////////
    // configure logging
    ///////////////////////////////////////////////////////////////////////////
    scc::init_logging(scc::LogConfig()
                          .logFileName(vm_["log-file"].as<std::string>())
                          .logLevel(vm_["verbose"].as<scc::log>())
                          .logFilterRegex(vm_["log-filter"].as<std::string>())
                          .logAsync(!vm_["log-sync"].as<bool>()));
    scc::stream_redirection cout_redir(std::cout, scc::log::DEBUG);
    scc::stream_redirection cerr_redir(std::cerr, scc::log::ERROR);
    sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", sc_core::SC_DO_NOTHING);
    sc_core::sc_report_handler::set_actions(sc_core::SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, sc_core::SC_DO_NOTHING);
    sc_core::sc_report_handler::set_actions(
        sc_core::SC_ERROR, sc_core::SC_LOG | sc_core::SC_CACHE_REPORT | sc_core::SC_DISPLAY | sc_core::SC_STOP);
}

void CLIParser::build() {
    // clang-format off
    desc.add_options()
            ("help,h",
                    "Print help message")
            ("verbose,v", po::value<scc::log>()->default_value(scc::log::INFO),
                    "debug output level (NONE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE, TRACEALL)")
            ("log-file,l", po::value<std::string>()->default_value(""),
                    "log file name")
            ("log-filter", po::value<std::string>()->default_value(""),
                    "log filter regular expression name")
            ("log-sync", po::bool_switch(),
                    "Disable asynchronous logging")
            ("disass,d", po::value<std::string>()->implicit_value(""),
                    "Enables disassembly")
            ("elf,l", po::value<std::string>(),
                    "ELF file to load")
            ("gdb-port,g", po::value<unsigned short>()->default_value(0),
                    "enable gdb server and specify port to use")
            ("dump-ir",
                    "dump the intermediate representation")
            ("quantum", po::value<unsigned>(),
                    "SystemC quantum time in ns")
            ("reset,r", po::value<std::string>(),
                    "reset address")
            ("trace-level,t", po::value<unsigned>()->default_value(0),
                    "enable tracing, or combination of 1=signals and 2=TX text, 4=TX compressed text, 6=TX in SQLite")
            ("trace-default-on",
                    "enables tracing for all unspecified modules")
            ("trace-file", po::value<std::string>()->default_value("system"),
                    "set th ename of the trace file")
            ("max_time,m", po::value<std::string>(),
                    "maximum time to run")
            ("config-file,c", po::value<std::string>()->default_value(""),
                    "read configuration from file")
            ("dump-config,dc", po::value<std::string>()->default_value(""),
                    "dump configuration to file file");
    // clang-format on
}

CLIParser::~CLIParser() = default;
