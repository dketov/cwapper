cmake_minimum_required(VERSION 2.6)
project(petshop)

find_library(LIB_BOOSTER booster)
find_library(LIB_CPPCMS cppcms)

set(CMAKE_CXX_FLAGS -std=c++11)

add_executable(api api.cpp)
target_link_libraries(api cppcms booster)
add_custom_command(
	OUTPUT api.cpp
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../cwapper.py ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml > api.cpp
)
