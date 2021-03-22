cmake_minimum_required(VERSION 3.12)

project(tgc-vp LANGUAGES C CXX VERSION 0.0.1)

###############################################################################
# SiFive
###############################################################################
if(TARGET CONAN_PKG::seasocks)
 	set(WEBSOCK_SRC
 	 	sifive/gpio.cpp
 	)
endif()
add_executable(${PROJECT_NAME}
	sc_main.cpp
	CLIParser.cpp
	tgfs_vp/tb.cpp
	tgfs_vp/system.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC tgc_sc vpvper_sifive)
