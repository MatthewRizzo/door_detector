# use this to find in main dir
add_library(GPIO_Controller
    gpio_controller.cpp
)

target_link_libraries(GPIO_Controller
    # WiringPi Requirements
    ${wiring_pi_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    crypt   # for undefined reference to `crypt'
    rt      # for undefined reference to `shm_open'
)

target_compile_options(GPIO_Controller
    PRIVATE
)