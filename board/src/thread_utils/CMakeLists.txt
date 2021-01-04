# use this to find in main dir
add_library(THREAD_UTILS
    thread_utils.cpp
)

target_link_libraries(THREAD_UTILS
)

target_compile_options(THREAD_UTILS
    PRIVATE
)