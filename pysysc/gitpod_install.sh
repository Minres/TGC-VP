#!/bin/sh
##

git clone https://github.com/Minres/PySysC.git /workspace/PySysC
SYSTEMC_HOME=`ls -d /home/gitpod/.conan/data/systemc/2.3.3/_/_/package/*/` pip3 install  /workspace/PySysC
pip install /workspace/TGC-VP/scc/contrib/pysysc/
