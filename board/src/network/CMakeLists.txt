# use this to find in main dir
add_library(NETWORK
    comm_utils.cpp
    confirm_handshake.cpp
    handshake_base.cpp
    handshake_controller.cpp
    recv_handshake.cpp
)

target_link_libraries(NETWORK
)