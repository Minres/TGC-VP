#!/bin/sh
##

# Install prerequisites
pip install wheel
# clone and install PySysC
git clone https://github.com/Minres/PySysC.git ../PySysC
STDCXX=11 SYSTEMC_HOME=`ls -d ~/.conan/data/systemc/2.3.3/_/_/package/* | head -1` pip install ../PySysC/
# install SCC integration
pip install scc/contrib/pysysc
