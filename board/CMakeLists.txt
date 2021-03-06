cmake_minimum_required(VERSION 2.8)
project (RPI)

# use c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# use gcc/++7
set(CMAKE_C_COMPILER /usr/bin/gcc-7 CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/g++-7 CACHE PATH "" FORCE)

# set build type based on makefile target
# CMAKE_BUILD_TYPE set in Makefile
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-psabi")
# -Wno-psabi removes warning for api differences between GCC 6.x and 7.1


set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set number of Processors & % usage
include(ProcessorCount)
ProcessorCount(N)
if(NOT N GREATER 1)
    # always use 1 less than available to not soft freeze computer
    math(EXPR PROC_TO_USE "${N}-1")
    set(CTEST_BUILD_FLAGS -j${PROC_TO_USE})
    set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${PROC_TO_USE})
endif()


# =================================== load Dependencies globally for all subdirs ===================================
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

# Find the packages
# find_package(BoostLib REQUIRED) # uses FindBoost.cmake in cmake dir
find_package(CLI11 REQUIRED) # using https://cliutils.gitlab.io/CLI11Tutorial/
find_package(wiring_pi REQUIRED)
find_package(Threads REQUIRED) # stock package needed for WiringPi -- link ${CMAKE_THREAD_LIBS_INIT}
find_package(JSON REQUIRED)

# Include the package's header files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/includes") # contains project defined headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/includes/network")

# external includes
include_directories(${CLI11_INCLUDE_DIR})
include_directories(${wiring_pi_INCLUDE_DIRS}) # pair with target_link_libraries(<projName> ${wiring_pi_LIBS})
include_directories(SYSTEM ${JSON_INCLUDE_DIRS}) # pair with target_link_libraries(<projName> ${wiring_pi_LIBS})

# ============================================ Build individual subdirs ============================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin) # store produce binaries in src/bin/*
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# add more folders here as needed
# these trigger the CMakeLists.txt file to be called in the listed dirs
add_subdirectory(src/cli)
add_subdirectory(src/gpio)
add_subdirectory(src/json)
add_subdirectory(src/main)
add_subdirectory(src/network)
add_subdirectory(src/thread_utils)
