# === CCEmu CMake file ===

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

# === FetchContent Init ===
include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# === Boost ===
find_package(Boost 1.74.0 COMPONENTS date_time)
if (NOT Boost_FOUND)
	FetchContent_Declare(
		Boost
		GIT_REPOSITORY https://github.com/boostorg/boost.git 
		GIT_TAG boost-1.74.0
	)
	FetchContent_MakeAvailable(Boost)
endif()

# === cpp-httplib ===
FetchContent_Declare(
	cpp-httplib
	GIT_REPOSITORY https://github.com/yhirose/cpp-httplib 
	)
FetchContent_MakeAvailable(cpp-httplib)

# === uuid_v4 ===
FetchContent_Declare (
	uuid_v4
	GIT_REPOSITORY https://github.com/crashoz/uuid_v4 
	)
FetchContent_MakeAvailable(uuid_v4)

# === nlohmann/json ===
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json
	)
FetchContent_MakeAvailable(json)

# === spdlog ===
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog
)

FetchContent_GetProperties(spdlog)
    if(NOT spdlog_POPULATED)
      FetchContent_Populate(spdlog)
      add_subdirectory(
        ${spdlog_SOURCE_DIR}
        ${spdlog_BINARY_DIR})
    endif()
FetchContent_MakeAvailable(spdlog)

# === gtest/gmock ===
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG 76bb2afb8b522d24496ad1c757a49784fbfa2e42
)
FetchContent_MakeAvailable(googletest)

# === Include directories ==	
include_directories(include/)
include_directories(thirdparty/**)

# === Adding executable and linking === 
add_executable(CCEmu ${CCEMU_SRC})
target_link_libraries(CCEmu PUBLIC ${BOOST_LIBRARIES} nlohmann_json::nlohmann_json spdlog 
					  httplib::httplib uuid_v4 gtest_main gmock_main)
