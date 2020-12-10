
# Find_wiring_pi.cmake - Try to find the WiringPi c++ library for RPI GPIOs
# Once done this will define
#
# wiring_pi_LIBS         - Location of WiringPi bins
# wiring_pi_INCLUDE_DIRS - Location of the WiringPi header

# paths originate from WiringPi ./build script (called from setup script)
find_library(wiring_pi_LIBS wiringPi)
find_path(wiring_pi_INCLUDE_DIRS wiringPi.h)

# check if the library exists (if not, need to call build script)
if (NOT wiring_pi_INCLUDE_DIRS)
    message(WARNING "WiringPi library not found -- calling build script")
    execute_process(
        # call from helper directly to avoid using sudo
        COMMAND ${CMAKE_SOURCE_DIR}/install/helpers/wiring_pi.sh --mode install
    )
    # try to find again now that it was installed
    find_library(wiring_pi_LIBS NAMES wiringPi)
    find_path(wiring_pi_INCLUDE_DIRS NAMES wiringPi.h)
endif()

MARK_AS_ADVANCED(wiring_pi_LIBS wiring_pi_INCLUDE_DIRS)