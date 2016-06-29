cmake_minimum_required(VERSION 2.6)
project(petshop)

find_library(LIB_BOOSTER booster)
find_library(LIB_CPPCMS cppcms)

set(CMAKE_CXX_FLAGS -std=c++11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(api api.cpp)
target_link_libraries(api cppcms booster)

add_custom_command(
	OUTPUT api.cpp
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../cwapper.py ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml > api.cpp
)

add_custom_target(master.zip
	COMMAND wget -c https://github.com/swagger-api/swagger-ui/archive/master.zip
)
	
add_custom_target(swagger-ui
	DEPENDS master.zip
	COMMAND 7z x -y master.zip
	COMMAND mv -n swagger-ui-master/dist ${CMAKE_CURRENT_SOURCE_DIR}/swagger-ui
)
add_dependencies(api swagger-ui)
