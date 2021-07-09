# TGC-VP
The Scale4Edge ecosystem VP using VP-VIBES peripherals.

This VP is based in MINRES TGC series cores and uses CoreDSL to generate the concrete ISS 
of a particular ISA + extensions. The generator approach makes it very flexible and adaptable.
Since the CoreDSL description is used to generate RTL as well as verification artifacts it 
provides a comprehensive and consistent solution to develop processor cores.

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
  
* create conan profile:
  
  ```sh

  conan profile create default --detect

  ```
  
* setup conan to use the minres repo:
  
  ```sh

  conan remote add minres https://artifactory.minres.com/artifactory/api/conan/oss

  ```
  
* checkout source from git
* start an out-of-source build:
  
  ```
  cd TGC-VP
  mkdir build
  cd build
  cmake ..
  cmake --build .
  ```
  
* if you encounter issues when linking wrt. c++11 symbols you might have run into GCC ABI incompatibility introduced from GCC 5.0 onwards. You can fix this by adding '-s compiler.libcxx=libstdc++11' to the conan call or changing compiler.libcxx to
  
  ```
  compiler.libcxx=libstdc++11
  ```
  
  in $HOME/.conan/profiles/default


