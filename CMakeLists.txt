cmake_minimum_required(VERSION 3.20)

project(CCEmu
        VERSION 1.0
        DESCRIPTION "Call-Center Emulator project for Protei IT School"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE CCEMU_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(include/)
include_directories(thirdparty/cpp-httplib)
include_directories(thirdparty/json/single_include)

add_executable(CCEmu ${CCEMU_SRC})

