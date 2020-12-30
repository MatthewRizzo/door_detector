
# Create the Handshake library and set it to private
add_library(HANDSHAKE
    comm_utils.cpp
    confirm_handshake.cpp
    handshake_base.cpp
    handshake_controller.cpp
    recv_handshake.cpp
)

target_compile_options(HANDSHAKE
    PRIVATE
)

# Create the udp library and set it to private
add_library(COMMS
    udp_sender.cpp
)
target_compile_options(COMMS
    PRIVATE
)