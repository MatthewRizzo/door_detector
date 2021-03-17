import flask
from flask import request, Flask, render_template, send_from_directory, jsonify, redirect, flash
from flask_socketio import SocketIO
from flask_login import login_user, current_user, login_required, logout_user
import logging
import secrets # Used to create a secure secret key (used by flask's application)
import os
import webbrowser

# Used to wrap App in a secure production server environment
import werkzeug.serving
from werkzeug.serving import run_simple

# -- Project Defined Imports -- #
from web_app import app_constants
from network.network_utils import NetworkUtils

class AppManager():
    # Used to determine if shutdown should be performed on ctrl+c
    """Class responsible for setting up the Flask app and managing all the routes / sockets.
    Start the app by calling start_app()"""
    _socketio = None
    _is_silent = False
    def __init__(self, debug: bool, port: int, is_silent=False):
        """
        :param debug: - If true, puts flask into debug mode
        :param port: The port the web app runs on the server
        :param is_silent Defaults to False. If set to true, all print's will be suppressed
        """
        AppManager._is_silent = is_silent
        self._is_window_open = False
        self._debug = debug
        self._app_port = port
        self._hostname, self._ip = NetworkUtils.get_cur_hostname_IP().values()


        self.sites = app_constants.SITE_PATHS
        self._setup_app_config()

        # Used to communicate from backend->frontend js
        AppManager._socketio = SocketIO(self.app, async_mode="threading")

        # Setup App Routes
        self._create_routes()

        # Done with setup. Inform user of anything they need
        self._update_homepage_url()
        print(f"The webapp can be accessed at {self._homepage_url}")

    @classmethod
    def kill_app(cls):
        """Asks the front end to send a shutdown request to shutdown the server"""
        # msg triggers a shutdown request from the Frontend
        if not cls._is_silent:
            print("Shutting down Web App")
        cls._send_to_client("shutdown", {})

    def start_app(self):
        """API function to actually start the App up"""
        self._update_homepage_url()

        if self._debug is False:
            self._log = logging.getLogger('werkzeug')
            self._log.setLevel(logging.ERROR)
        else:
            self._log = logging.getLogger('werkzeug')
            self._log.setLevel(logging.INFO)
        if not AppManager._is_silent:
            print(f"Starting Web App Notification at this link: {self._homepage_url}")

        webbrowser.open(self._homepage_url)

        # run the app
        self._is_window_open = True
        # self.app.run(host=self._ip, port=self._app_port, debug=self._debug)
        run_simple(hostname=self._ip, port=self._app_port, application=self.app, use_reloader=self._debug, threaded=True)

    @property
    def ip(self):
        return self._ip

    @property
    def port(self):
        return self._app_port

    @property
    def log(self):
        return self._log

    def _create_routes(self):
        self._create_mainpage_routes()

        self._create_end_routes()


    def _create_mainpage_routes(self):
        @self.app.route(self.sites["mainpage"])
        def mainpage():
            return render_template("index.html")

        @self.app.route(self.sites["icon"])
        def favicon():
            return send_from_directory(self._images_dir, 'ajar_door.jpg', mimetype='image/vnd.microsoft.icon')

    def _create_end_routes(self):
        @self.app.route(self.sites["shutdown"], methods=['POST'])
        def shutdown_wrapper():
            if not AppManager._is_silent:
                print("\n------------------------")
                print("Starting shutdown of Web Application")
            func = request.environ.get('werkzeug.server.shutdown')
            if func is None:
                raise RuntimeError('Not running with the Werkzeug Server')
            func()
            if not AppManager._is_silent:
                print("Shutdown of Web Application Complete")
                print("------------------------\n")
            return 'Server shutting down...'

    def _setup_app_config(self):

        self._web_app_dir_path = os.path.abspath(os.path.dirname(__file__)) # web_app/

        self._src_root = os.path.dirname(self._web_app_dir_path) # server/
        self._frontend_dir = os.path.join(self._web_app_dir_path, "frontend") # frontend/
        self._static_dir = os.path.join(self._frontend_dir, "static")
        self._template_dir = os.path.join(self._frontend_dir, "templates")
        self._images_dir = os.path.join(self._static_dir, "images")

        if self._debug:
            print(f"Path to templates: {self._template_dir}")
            print(f"Path to static: {self._static_dir}")

        self._url_root = "Door Monitor"
        self.app = Flask(__name__,
                    static_folder=self._static_dir,
                    template_folder=self._template_dir,
                    root_path=self._src_root)
        self.app.config['TEMPLATES_AUTO_RELOAD'] = True

    def _get_site_url(self, ip: str, extension: str, port: str) -> str:
        """:brief Given the url extension after http://ip:, returns the complete url for that site"""
        return f"http://{ip}:{port}{extension}"

    @classmethod
    def _send_to_client(cls, message_name, content_json=None):
        """Function to enable communication from backend to front-end via sockets"""
        # Only send to client when socket exists
        if cls._socketio is None:
            return
        if content_json:
            cls._socketio.emit(message_name, content_json)
        else:
            cls._socketio.emit(message_name)

    def _update_homepage_url(self):
        self._homepage_url = self._get_site_url(self._ip, self.sites["mainpage"], self._app_port)