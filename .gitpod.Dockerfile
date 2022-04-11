FROM gitpod/workspace-full:latest


# Make a working folder and set the necessary environment variables.
ENV RISCV /opt/riscv
RUN sudo mkdir -p $RISCV
RUN sudo touch $RISCV/install.stamp

# Add the GNU utils bin folder to the path.
ENV PATH $RISCV/bin:$PATH
MAINTAINER eyck@minres.com 

USER root
RUN apt-get install gnupg software-properties-common wget -y
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null |  apt-key add - 
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
 
# Obtain the RISCV-tools repo which consists of a number of submodules
# so make sure we get those too.
WORKDIR /opt/riscv
RUN apt-get update && \
	apt-get install -y autoconf automake autotools-dev curl python3 python3-pip python3-venv libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev cmake gdb && \
	apt-get clean && rm -rf /var/lib/apt/lists/*
RUN git clone --recursive https://github.com/riscv/riscv-gnu-toolchain && \
  cd riscv-gnu-toolchain && \
    ./configure --prefix=/opt/riscv --without-system-zlib --enable-multilib --with-arch=rv32gc --with-abi=ilp32d && \
	make -j newlib && \
  cd .. && \
    rm -rf riscv-gnu-toolchain
RUN apt-get update && apt install default-jdk -y
USER gitpod
