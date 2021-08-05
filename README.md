# TGC-VP
The Scale4Edge ecosystem VP using VP-VIBES peripherals.

This VP is based in MINRES TGC series cores and uses CoreDSL to generate the concrete ISS 
of a particular ISA + extensions. The generator approach makes it very flexible and adaptable.
Since the CoreDSL description is used to generate RTL as well as verification artifacts it 
provides a comprehensive and consistent solution to develop processor cores.

## Ultra Quick start

Using gitpod you can run the VP in the cloud. Just visit https://www.gitpod.io/#https://github.com/Minres/TGC-VP and follow the insturctions.
After the build finished you can run

```
src/tgc-vp -f ../fw/hello-world/hello.elf 
```

You will see on console the prints of the hello world firmware at fw/hello-world/hello.c

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
  
* checkout source from git

* start an out-of-source build:
  
  ```
  cd TGC-VP
  mkdir build
  cd build
  cmake ..
  make -j tgc-vp
  ```
  
* run the VP with pre-built frimware

  ```
  src/tgc-vp -f ../fw/hello-world/hello.elf 
  ```
  
To rebuild the firmware you need to install a RISC-V toolchain like https://github.com/riscv/riscv-tools.