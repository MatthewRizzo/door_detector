# use this to find in main dir
add_library(JSON_UTILS
    json_utils.cpp
)

target_link_libraries(JSON_UTILS
)

target_compile_options(JSON_UTILS
    PRIVATE
)