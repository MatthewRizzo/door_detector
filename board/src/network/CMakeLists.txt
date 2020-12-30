# use this to find in main dir
add_library(NETWORK
    HANDSHAKE
    UDP
)

add_library(HANDSHAKE
    comm_utils.cpp
    confirm_handshake.cpp
    handshake_base.cpp
    handshake_controller.cpp
    recv_handshake.cpp
)

add_library(UDP
    udp_sender.cpp
)

target_link_libraries(NETWORK
)