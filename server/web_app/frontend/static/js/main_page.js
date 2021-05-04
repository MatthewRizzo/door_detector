"using strict"

import { async_post_request, post_request, async_get_request } from './utils.js'
import { create_socket_listener } from './server-messages.js'

$(document).ready(() => {

    // Get the status from the backend after the page loads
    window.onload = async (event) => {
        await check_block_status();
    };

    // make a listener for the shutdown request
    create_socket_listener("shutdown", perform_shutdown);
    create_socket_listener("block", start_block);

    const shutdown_button = document.getElementById("shutdown-button");
    shutdown_button.addEventListener('click', end_block)
})

/**
 * @brief Runs the seris of commands / requests to shutdown the App
 */
function perform_shutdown()
{
    console.log("Shutdown requested")
    post_request("/shutdown", {});
    document.getElementById('after-shutdown-msg').style.display = "block";
    // window.close();
}


/**
 * @brief Check's what the block status for the ack should be and updates the page
 */
async function check_block_status()
{
    // const is_blocked = await async_get_request("/info/block_status")["block_status"];
    const is_blocked_raw = await async_get_request("/info/block_status");
    const is_blocked = is_blocked_raw.block_status;

    // display the "this page can be closed" msg if the backend is blocking for it
    const display_status = is_blocked == true ? "none" : "display";

    document.getElementById('after-shutdown-msg').style.display = display_status;
    return is_blocked;
}

/**
 * @brief When the door opens, change the display of the page to expect an ack.
 * Blocks the server
 */
function start_block()
{
    document.getElementById('after-shutdown-msg').style.display = "none";
}

/**
 * @brief When the ack button is pressed, end the block of the server getting updates from board
 */
function end_block()
{
    post_request("/end_block", {})
    document.getElementById('after-shutdown-msg').style.display = "block";
}