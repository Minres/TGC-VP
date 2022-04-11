#%Module######################################################################
##
##      Project Module
##
proc ModulesHelp { } {
  puts stderr "\tThe TGC verification project Module\n"
  puts stderr "\tThis module loads PATHs and variables for running TGC verification."
}

set distro [exec /usr/bin/lsb_release -i -s]
if { $distro == "CentOS" && ![info exists ::env(PROJECT)] && ![info exists ::env(PCP_DIR)] } {
    puts stderr     "Don't forget to execute 'scl enable devtoolset-7 llvm-toolset-7 bash'"
}

module load tools/gcc-riscv32/9.2.0
module load tools/cmake
