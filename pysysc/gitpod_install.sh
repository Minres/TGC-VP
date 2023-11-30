#!/bin/sh
##

# Install prerequisites
pip install wheel
# clone and install PySysC
git clone https://github.com/Minres/PySysC.git /workspace/PySysC
STDCXX=14 SYSTEMC_HOME=`ls -d $CONAN_USER_HOME/.conan/data/systemc/2.3.3/_/_/package/* | head -1` \
  pip install /workspace/PySysC/
# install SCC integration
pip install scc/contrib/pysysc
