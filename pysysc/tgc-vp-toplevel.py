#
# Copyright (c) 2019 -2021 MINRES Technolgies GmbH
#
# SPDX-License-Identifier: Apache-2.0
#

import os.path
import logging
import cppyy
from cppyy import gbl as cpp
import pysysc
import pysysc.scc as scc
from pysysc.structural import Module, Simulation

###############################################################################
# Include and load section
###############################################################################
logging.basicConfig(level=logging.INFO)
###############################################################################
current_dir = os.path.dirname(os.path.realpath(__file__))
project_dir = os.path.dirname(current_dir)
logging.info(f"Project dir: {project_dir}")
if os.path.isdir(os.path.join(project_dir, 'build/Debug')):
    pysysc.read_config_from_conan(os.path.join(project_dir, 'build/Debug'), 'Debug')
elif os.path.isdir(os.path.join(project_dir, 'build/Release')):
    pysysc.read_config_from_conan(os.path.join(project_dir, 'build/Release'), 'Release')
elif os.path.isdir(os.path.join(project_dir, 'build')):
    pysysc.read_config_from_conan(os.path.join(project_dir, 'build'), 'Release')
else:
    sys.exit("Could not find build dir")
    
pysysc.load_systemc()
logging.root.setLevel(logging.DEBUG)
###############################################################################
scc.load_lib(project_dir)
###############################################################################
logging.debug("Loading TGC-VP Peripherals libs")
pysysc.add_include_path(os.path.join(project_dir, 'vpvper'))
pysysc.add_library('sifive.h', 'libvpvper_sifive.so', project_dir)
###############################################################################
logging.debug("Loading TGC-ISS")
pysysc.add_include_path(os.path.join(project_dir, 'tgc-iss/dbt-rise-tgc/incl/sysc'))
pysysc.add_library('core_complex.h', 'libdbt-rise-tgc_sc.so', project_dir)

###############################################################################
# Include section
###############################################################################
cppyy.include(os.path.join(project_dir, 'src/tgfs_vp/rst_gen.h'))
###############################################################################
# configure
###############################################################################
scc.setup(logging.root.level)
scc.configure(enable_vcd=True)
###############################################################################
# instantiate
###############################################################################
from modules import TopModule
dut = Module(TopModule).create("dut")
# Load FW         
dut.core_complex.instance.elf_file.set_value(os.path.join(project_dir, 'fw/hello-world/prebuilt/hello.elf'))
#dut.core_complex.instance.enable_disass.set_value(True)
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    Simulation.run()
    logging.debug("Done")
    
