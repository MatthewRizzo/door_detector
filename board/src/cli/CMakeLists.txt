# use this to find in main dir
add_library(CLI_CONTROLLER
    cli_controller.cpp
)

target_link_libraries(CLI_CONTROLLER
)

target_compile_options(CLI_CONTROLLER
    PRIVATE
)