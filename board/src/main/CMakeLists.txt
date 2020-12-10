cmake_minimum_required(VERSION 2.8)

add_executable(main_driver
    main.cpp
)

# include created libs from other dirs -- comes after add executable
target_link_libraries(main_driver
    CLIENT
    GPIO_CLI_Processor
    GPIO_Controller
)