/*
 * Copyright (c) 2019 -2021 MINRES Technolgies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PLATFORM_SRC_CLIPARSER_H_
#define PLATFORM_SRC_CLIPARSER_H_

#include <boost/program_options.hpp>
#include <memory>

class CLIParser {
public:
    CLIParser(int argc, char *argv[]);

    virtual ~CLIParser();

    bool is_valid() { return valid; }

    const boost::program_options::variables_map &vm() { return vm_; }

    bool is_set(const char *option) { return vm_.count(option) != 0; }

    template <typename T> const T &get(const char *option) { return vm_[option].as<T>(); }

private:
    void build();
    bool valid;
    boost::program_options::variables_map vm_;
    boost::program_options::options_description desc;
};

#endif /* PLATFORM_SRC_CLIPARSER_H_ */
