FROM gitpod/workspace-base:latest

ENV RISCV /opt/riscv
RUN mkdir -p $RISCV
RUN touch $RISCV/install.stamp

# Add the GNU utils bin folder to the path.
ENV PATH $RISCV/bin:$PATH

# Obtain the RISCV-tools repo which consists of a number of submodules
# so make sure we get those too.
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-tools.git && \
  cd riscv-tools && \
    git submodule update --init --recursive && \
    sed -i 's/JOBS=16/JOBS=1/' build.common && \
    ./build.sh && \
  cd .. && \
    rm -rf riscv-tools