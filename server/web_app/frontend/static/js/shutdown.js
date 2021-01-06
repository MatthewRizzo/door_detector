"using strict"

import { post_request } from './utils.js'
import { create_socket_listener } from './server-messages.js'

$(document).ready(() => {
    // make a listener for the shutdown request
    create_socket_listener("shutdown", perform_shutdown);

    const shutdown_button = document.getElementById("shutdown-button");
    shutdown_button.addEventListener('click', perform_shutdown)
})

/**
 * @brief Runs the seris of commands / requests to shutdown the App
 */
function perform_shutdown()
{
    console.log("Shutdown requested")
    // post_request("/shutdown", {});
    const url = "/shutdown";
    $.post({
        url: url,
        data: JSON.stringify({}),
        contentType: 'application/json',

        success: function(){},
        error: function(request, status, error){
            console.log("Failed post request for url " + url);
            console.log("Error " + error);
            console.log("Status " + status);
        }
    });
    document.getElementById('after-shutdown-msg').style.display = "block";
    // window.close();
}