cmake_minimum_required(VERSION 3.20)

project(CCEmu
        VERSION 1.0
        DESCRIPTION "Call-Center Emulator project for Protei IT School"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

file(GLOB_RECURSE CCEMU_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS date_time)

if(Boost_FOUND)
        include_directories(include/)
        include_directories(thirdparty/cpp-httplib)
        include_directories(thirdparty/json/single_include/)
        include_directories(thirdparty/spdlog/include/)
        include_directories(thirdparty/uuid_v4/)


        add_executable(CCEmu ${CCEMU_SRC})
        target_link_libraries(CCEmu ${BOOST_LIBRARIES})
endif()
