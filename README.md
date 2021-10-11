# TGC-VP
The Scale4Edge ecosystem VP using VP-VIBES peripherals.

This VP is based in MINRES TGC series cores and uses CoreDSL to generate the concrete ISS 
of a particular ISA + extensions. The generator approach makes it very flexible and adaptable.
Since the CoreDSL description is used to generate RTL as well as verification artifacts it 
provides a comprehensive and consistent solution to develop processor cores.

## Ultra Quick start

Using gitpod you can run the VP in the cloud. Just visit [Gitpod.io](https://www.gitpod.io/#https://github.com/Minres/TGC-VP/tree/develop)
and follow the instructions. After the build finished you can run

```

build/src/tgc-vp -f fw/hello-world/prebuilt/hello.elf

```

or use ctest:

```

cd build
ctest

```


You will see on console the prints of the hello world firmware at fw/hello-world/hello.c

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/from-referrer)

## Quick start

* you need to have a C++14 capable compiler, make, python, and cmake installed

* install conan.io (see also http://docs.conan.io/en/latest/installation.html):
  
  ```

  pip3 install --user conan

  ```
  
  It is advised to use conan version 1.36 or newer. In case of an older version please run
  
  ```sh

  pip3 install --user --upgrade conan

  ``` 
  
  Installing conan for the first time you need to create a profile:
  
  ```
  
  conan profile create default --detect
  
  ```
  
* checkout source from git

  ```sh

  git clone --recursive -b develop https://github.com/Minres/TGC-VP.git 

  ``` 

* start an out-of-source build:
  
  ```

  cd TGC-VP
  mkdir build
  cd build
  cmake ..
  make -j tgc-vp

  ```
  
* run the VP with pre-built firmware

  ```

  ctest

  ```

  or

  ```

  src/tgc-vp -f ../fw/hello-world/prebuild/hello.elf 

  ```
  
To rebuild the firmware you need to install a RISC-V toolchain like https://github.com/riscv/riscv-tools.

# Windows/Visual Studio build

TGC-VP supports VS2019/MSVC 16 and has been tested only with this version

## Prerequisites

You need to have to following installed:

* Visual Studio 2019 (community edition is sufficient) with C/C++ support
* Python 3.6 or newer
* SystemC 2.3.3 and SystemC-CCI 1.0.0. Both should be installed into the same location and the 
  environment variable SYSTEMC_HOME should point to it
* Conan (https://conan.io/) version 1.36 or newer. If python is installed this can be installed using pip
  (see above in [Quick start](#quick-start))
  
## build step

Create a project at the location of the git workarea. This can be done 

```

devenv <path to checkout>

```

The build steps are as described in the VS documentation for CMake based projects.
  
