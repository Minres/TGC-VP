from cppyy import gbl as cpp
from pysysc.structural import Connection, Module, Signal, Clock, Simulation
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
        self.aon   = Module(cpp.vpvper.sifive.aon).create("aon")
        self.clint = Module(cpp.vpvper.sifive.clint).create("clint")
        self.uart  = Module(cpp.vpvper.sifive.uart_terminal).create("uart0")
        
        self.mem_qspi = Module(cpp.scc.memory[2**24,32]).create("mem_qspi")
        self.mem_ram  = Module(cpp.scc.memory[1024,32]).create("mem_ram")
        self.core_complex = Module(cpp.sysc.tgfs.core_complex).create("core_complex")
        self.router = Module(cpp.scc.router[32]).create("router", 5)
        ###############################################################################
        # connect them
        ###############################################################################
        Clock("clock", 64.5, 'SC_NS')\
        .sink(self.aon.clk_i)\
        .sink(self.clint.tlclk_i)\
        .sink(self.core_complex.clk_i)

        self.lfclk = Signal("lfclk").src(self.aon.lfclkc_o).sink(self.clint.lfclk_i)
    
        self.rst = Signal("rst").src(self.aon.rst_o)\
        .sink(self.clint.rst_i)\
        .sink(self.uart.rst_i)\
        .sink(self.core_complex.rst_i)

        Signal("erst_s").src(self.rst_gen.rst_n).sink(self.aon.erst_n_i)

        # Interrupts
        Signal("timer_irq_s").src(self.clint.mtime_int_o).sink(self.core_complex.timer_irq_i)
        Signal("sw_irq_s").src(self.clint.msip_int_o).sink(self.core_complex.sw_irq_i)
        Signal("core_irq_s").src(self.uart.irq_o).sink(self.core_complex.global_irq_i)
        [Signal(f"local_irq_{idx}").sink(self.core_complex.local_irq_i.at(idx)) for idx in range(self.core_complex.local_irq_i.size())]
                
        # Routing
        Connection().src(self.core_complex.initiator).sink(self.router.target.at(0))
        Connection().src(self.router.initiator.at(0)).sink(self.clint.socket)
        self.router.set_target_range(0, 0x2000000, 0xc000)
        Connection().src(self.router.initiator.at(1)).sink(self.aon.socket)
        self.router.set_target_range(1, 0x10000000, 0x150)
        Connection().src(self.router.initiator.at(2)).sink(self.mem_qspi.target)
        self.router.set_target_range(2, 0x20000000, 2**24)
        Connection().src(self.router.initiator.at(3)).sink(self.mem_ram.target)
        self.router.set_target_range(3, 0x80000000, 1024)   
        Connection().src(self.router.initiator.at(4)).sink(self.uart.socket)
        self.router.set_target_range(4, 0x10013000, 0x1c)
        
    def EndOfElaboration(self):
        print("Elaboration finished")
        
    def StartOfSimulation(self):
        print("Simulation started")
        
    def EndOfSimulation(self):
        print("Simulation finished")

