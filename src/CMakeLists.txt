#
# Copyright (c) 2019 -2021 MINRES Technolgies GmbH
#
# SPDX-License-Identifier: Apache-2.0
#
cmake_minimum_required(VERSION 3.12)
project(tgc-vp LANGUAGES C CXX VERSION 0.0.1)

include(flink)

find_package(Boost COMPONENTS program_options thread REQUIRED)
###############################################################################
# TGC
###############################################################################
set(CMAKE_INSTALL_RPATH $ORIGIN/../${CMAKE_INSTALL_LIBDIR})
add_executable(${PROJECT_NAME}
	sc_main.cpp
	CLIParser.cpp
	tgc_vp/tb.cpp
	tgc_vp/system.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_force_link_libraries(${PROJECT_NAME} PUBLIC dbt-rise-tgc_sc)
target_link_libraries(${PROJECT_NAME} PUBLIC vpvper_generic vpvper_minres ${BOOST_program_options_LIBRARY})
if(TARGET Boost::program_options)
	target_link_libraries(${PROJECT_NAME} PUBLIC Boost::program_options Boost::thread)
else()
	target_link_libraries(${PROJECT_NAME} PUBLIC ${BOOST_program_options_LIBRARY})
endif()

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # static lib
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # binaries
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # shared lib
  FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR} # for mac
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}  # headers for mac (note the different component -> different package)
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}             # headers
)
