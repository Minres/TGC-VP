cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scc/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tgc-iss/dbt-rise-tgc/cmake/)

project(ecosystem-vp LANGUAGES C CXX VERSION 1.0.0)

option(ENABLE_SCV "Enable the use of SCV" ON)
option(LIBS_ONLY "Just build the shared libraries needed to build the VP" OFF)
option(FW_BUILD "Enable the automatic download and build of some firmware to run on the VP" OFF)
option(EN_EXT_DEBUG "Enable extended debug output at runtime" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZER "Enable address sanitizer" OFF)
option(ENABLE_CLANGTIDY "Enable static analysis with clang-tidy." OFF)
option(WITH_TCC "Build TCC backend" ON)
option(WITH_LLVM "Build LLVM backend" OFF)
option(WITH_ASMJIT "Build ASMJIT backend" ON)

set(SCC_LIB_ONLY ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

# add address sanitizer
if(ENABLE_SANITIZER)
  set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options(/vmg /wd26812) # suppress Warnung C26812: "enum class" gegenüber "enum" (Enum.3) bevorzugen
endif()

find_package(tcc QUIET)
find_package(elfio)
find_package(fmt)
find_package(spdlog)
find_package(yaml-cpp)
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost REQUIRED COMPONENTS program_options QUIET)
include(SystemCPackage)

set(CLANG_FORMAT_EXCLUDE_PATTERNS scc build .direnv)
find_package(ClangFormat)

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(COVERAGE_EXCLUDES "${tgfs_verif_SOURCE_DIR}/scc/*" "${tgfs_verif_SOURCE_DIR}/build/*" "$ENV{HOME}/.conan2/*" "/opt/*")
endif()

if(ENABLE_CLANGTIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if (CLANG_TIDY_EXE)
      	message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CLANG_TIDY_CHECKS 
        "-*,modernize-*,-modernize-use-trailing-return-type,clang-analyzer-*,concurrency-*,cppcoreguidelines-*,boost-*,bugprone-*,performance-*,portability-*,readability-*")
        #set(CLANG_TIDY_CHECKS "*")
        set(CMAKE_CXX_CLANG_TIDY 
                ${CLANG_TIDY_EXE};
                -header-filter=${tgfs_verif_SOURCE_DIR};
                -checks=${CLANG_TIDY_CHECKS};)
    else()
      	message(AUTHOR_WARNING "clang-tidy not found!")
      	set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
    endif()
endif ()

add_subdirectory(scc)
add_subdirectory(tgc-iss/dbt-rise-core)
add_subdirectory(tgc-iss/dbt-rise-tgc)
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tgc-iss//dbt-rise-plugins)
	add_subdirectory(tgc-iss/dbt-rise-plugins)
endif()
if(NOT USE_CWR_SYSTEMC)
	add_subdirectory(vpvper)
endif()
if(NOT LIBS_ONLY)
	add_subdirectory(src)
endif()

if(FW_BUILD)
	include(FetchContent)
	set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/..)
	FetchContent_Declare(
	  	riscvfw
	  	GIT_REPOSITORY https://git.minres.com/Firmware/Firmwares.git
	  	GIT_TAG        main
	  	GIT_SHALLOW OFF
	  	UPDATE_DISCONNECTED ON
	)
	FetchContent_GetProperties(riscvfw)
	if(NOT riscvfw_POPULATED)
	  	FetchContent_Populate(riscvfw)
	endif()
	set(BOARD tgc_vp)
 	add_subdirectory(${riscvfw_SOURCE_DIR})
endif()

install(FILES ${CMAKE_CURRENT_LIST_DIR}/fw/hello-world/prebuilt/hello.elf DESTINATION share/tgc-vp)

# CTest is a testing tool that can be used to test your project.
enable_testing()
add_test(NAME tgc-vp-hello-world
         #WORKING_DIRECTORY ${ecosystem-vp_DIR}/bin
         COMMAND tgc-vp -f ${CMAKE_CURRENT_LIST_DIR}/fw/hello-world/prebuilt/hello.elf)
