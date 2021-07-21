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
from pysysc.structural import Connection, Module, Signal, Simulation

###############################################################################
# Include and load section
###############################################################################
logging.basicConfig(level=logging.DEBUG)
build_type='Debug'
###############################################################################
current_dir = os.path.dirname( os.path.realpath(__file__))
project_dir = os.path.dirname(current_dir)
logging.info(f"Project dir: {project_dir}")
pysysc.read_config_from_conan(os.path.join(project_dir, 'conanfile.txt'), build_type)
pysysc.load_systemc()
###############################################################################
logging.debug("Loading SC-Components lib")
pysysc.add_include_path(os.path.join(project_dir, 'scc/src/common'))
pysysc.add_library('scc_util.h', os.path.join(project_dir, 'build/%s/lib/libscc-util.so'%build_type))
pysysc.add_include_path(os.path.join(project_dir, 'scc/third_party'))
pysysc.add_include_path(os.path.join(project_dir, 'scc/src/sysc'))
pysysc.add_library('scc_sysc.h', os.path.join(project_dir, 'build/%s/lib/libscc-sysc.so'%build_type))
pysysc.add_include_path(os.path.join(project_dir, 'scc/src/components'))
###############################################################################
logging.debug("Loading TGC-VP Peripherals libs")
pysysc.add_include_path(os.path.join(project_dir, 'vpvper'))
pysysc.add_library('sifive.h', os.path.join(project_dir, f'build/{build_type}/lib/libvpvper_sifive.so'))
###############################################################################
logging.debug("Loading TGC-ISS")
pysysc.add_include_path(os.path.join(project_dir, 'tgc-iss/dbt-rise-tgc/incl/sysc'))
pysysc.add_library('core_complex.h', os.path.join(project_dir, f'build/{build_type}/tgc-iss/dbt-rise-tgc/lib/libdbt-core-tgc_sc.so'))

###############################################################################
# Include section
###############################################################################
cppyy.include('scc/memory.h')
cppyy.include('scc/router.h')
cppyy.include(os.path.join(project_dir, 'src/tgfs_vp/rst_gen.h'))
###############################################################################
# define toplevel class
###############################################################################
class TopModule(cpp.scc.PyScModule):
    
    def __init__(self, name):
        super().__init__(self, name)
        ###############################################################################
        # instantiate
        ###############################################################################
        self.rst_gen = Module(cpp.tgfs_vp.rst_gen).create("rst_gen")
        self.prci  = Module(cpp.vpvper.sifive.prci).create("prci")
        self.aon   = Module(cpp.vpvper.sifive.aon).create("aon")
#        self.gpio  = Module(cpp.vpvper.sifive.gpio).create("gpio")
        self.plic  = Module(cpp.vpvper.sifive.plic).create("plic")
        self.clint = Module(cpp.vpvper.sifive.clint).create("clint")
        self.pwms  = [Module(cpp.vpvper.sifive.pwm).create(f"pwm{idx}") for idx in range(3)]
#        self.uarts = [Module(cpp.vpvper.sifive.uart).create(f"uart{idx}") for idx in range(0,1)]
        
        self.mem_qspi = Module(cpp.scc.memory[2**24,32]).create("mem_qspi")
        self.mem_ram  = Module(cpp.scc.memory[1024,32]).create("mem_ram")
        self.core_complex = Module(cpp.sysc.tgfs.core_complex).create("core_complex")
        self.router = Module(cpp.scc.router[32]).create("router", 9)
        ###############################################################################
        # connect them
        ###############################################################################
        self.clk = Signal("clk")\
        .src(self.prci.hfclk_o)\
        .sink(self.aon.clk_i)\
        .sink(self.plic.clk_i)\
        .sink(self.clint.tlclk_i)\
        .sink(self.core_complex.clk_i)
#        .sink(self.gpio.clk_i)\

        [self.clk.sink(i.clk_i) for i in self.pwms]
#        [self.clk.sink(i.clk_i) for i in self.uarts]

        self.lfclk = Signal("lfclk").src(self.aon.lfclkc_o).sink(self.clint.lfclk_i)
    
        self.rst = Signal("rst").src(self.aon.rst_o)\
        .sink(self.plic.rst_i)\
        .sink(self.prci.rst_i)\
        .sink(self.clint.rst_i)\
        .sink(self.core_complex.rst_i)
#        .sink(self.gpio.rst_i)\

        [self.rst.sink(i.rst_i) for i in self.pwms]
#        [self.rst.sink(i.rst_i) for i in self.uarts]
        Signal("erst_s").src(self.rst_gen.rst_n).sink(self.aon.erst_n_i)

        # Interrupts
        Signal("timer_irq_s").src(self.clint.mtime_int_o).sink(self.core_complex.timer_irq_i)
        Signal("sw_irq_s").src(self.clint.msip_int_o).sink(self.core_complex.sw_irq_i)
        Signal("core_irq_s").src(self.plic.core_interrupt_o).sink(self.core_complex.global_irq_i)
# warum wird SignalVector nicht gefunden?        SignalVector("local_irq_s").sink(self.core_complex.local_irq_i)
        self.global_irq = [Signal(f"global_irq_{idx}").sink(self.plic.global_interrupts_i.at(idx)) for idx in range(self.plic.global_interrupts_i.size())]
        [Signal(f"local_irq_{idx}").sink(self.core_complex.local_irq_i.at(idx)) for idx in range(self.core_complex.local_irq_i.size())]
        
        self.pwms[0].cmpip_o[0].bind(self.global_irq[40].signal)
        self.pwms[0].cmpip_o[1].bind(self.global_irq[41].signal)
        self.pwms[0].cmpip_o[2].bind(self.global_irq[42].signal)
        self.pwms[0].cmpip_o[3].bind(self.global_irq[43].signal)

        self.pwms[1].cmpip_o[0].bind(self.global_irq[44].signal)
        self.pwms[1].cmpip_o[1].bind(self.global_irq[45].signal)
        self.pwms[1].cmpip_o[2].bind(self.global_irq[46].signal)
        self.pwms[1].cmpip_o[3].bind(self.global_irq[47].signal)

        self.pwms[2].cmpip_o[0].bind(self.global_irq[48].signal)
        self.pwms[2].cmpip_o[1].bind(self.global_irq[49].signal)
        self.pwms[2].cmpip_o[2].bind(self.global_irq[50].signal)
        self.pwms[2].cmpip_o[3].bind(self.global_irq[51].signal)
        
        # Routing
        Connection().src(self.core_complex.initiator).sink(self.router.target.at(0))
        Connection().src(self.router.initiator.at(0)).sink(self.clint.socket)
        self.router.set_target_range(0, 0x2000000, 0xc000)
        Connection().src(self.router.initiator.at(1)).sink(self.plic.socket)
        self.router.set_target_range(0, 0xc000000, 0x200008)
        Connection().src(self.router.initiator.at(2)).sink(self.aon.socket)
        self.router.set_target_range(0, 0x10000000, 0x150)
        Connection().src(self.router.initiator.at(3)).sink(self.prci.socket)
        self.router.set_target_range(0, 0x10008000, 0x14)
        Connection().src(self.router.initiator.at(4)).sink(self.pwms[0].socket)
        self.router.set_target_range(0, 0x10015000, 0x30)
        Connection().src(self.router.initiator.at(5)).sink(self.pwms[1].socket)
        self.router.set_target_range(0, 0x10025000, 0x30)
        Connection().src(self.router.initiator.at(6)).sink(self.pwms[2].socket)
        self.router.set_target_range(0, 0x10035000, 0x30)
        Connection().src(self.router.initiator.at(7)).sink(self.mem_qspi.target)
        self.router.set_target_range(0, 0x20000000, 2**24)
        Connection().src(self.router.initiator.at(8)).sink(self.mem_ram.target)
        self.router.set_target_range(0, 0x80000000, 1024)   
        
    def EndOfElaboration(self):
        print("Elaboration finished")
        
    def StartOfSimulation(self):
        print("Simulation started")
        
    def EndOfSimulation(self):
        print("Simulation finished")

###############################################################################
# configure
###############################################################################
Simulation.setup(logging.root.level)
###############################################################################
# instantiate
###############################################################################
#from modules import TopModule
dut = Module(TopModule).create("dut")
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    Simulation.configure(enable_vcd=False)
    Simulation.run()
    logging.debug("Done")