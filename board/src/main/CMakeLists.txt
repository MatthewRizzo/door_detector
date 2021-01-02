cmake_minimum_required(VERSION 2.8)

add_executable(main_driver
    main.cpp
)

# include created libs from other dirs -- comes after add executable
target_link_libraries(main_driver
    CLI_CONTROLLER
    JSON_UTILS
    GPIO_Controller
    HANDSHAKE
    COMMS
    THREAD_UTILS
)

target_compile_options(main_driver
    PRIVATE
)