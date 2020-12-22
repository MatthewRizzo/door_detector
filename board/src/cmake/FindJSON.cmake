# FindJSONOpts.cmake - Try to find the CLI library
# Once done this will define
#
# JSONOPTS_FOUND       - Found the JSONopts path
# JSONOPTS_INCLUDE_DIR - Location of the cxxopts header

# Can set using a static path because these are tagged by git and not version controlled
SET(JSON_FOUND True)
# message(STATUS "source dir ${CMAKE_CURRENT_SOURCE_DIR}")
SET(JSON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/extern/json/) # give relative path to include dir