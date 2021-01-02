import platform
import re
import socket
import subprocess

class NetworkUtils:
    """Util class for static utility functions that can
        be used across other classes.
    """
        # Class methods to handle multicasting / multicasting to Pi
    @classmethod
    def  _platform_check(cls, is_windows: bool = True) -> bool:
        """Utility function that checks what the platform is.
        \n is_windows - If true (defualt), assumes platform is windows and checks if true. If false, checks if is linux
        """
        expected_platform = "Windows" if is_windows else "Linux"
        return expected_platform == platform.system()

    @classmethod
    def get_cur_hostname_IP(cls)->dict:
        """Return the IP address of your machine as seen by the router
        \n{'hostname' : hostname, 'ip' : ip_addr}"""
        if(cls._platform_check(True)):
            hostname = socket.gethostname()
            ip_addr = socket.gethostbyname(hostname)
            return {'hostname' : hostname, 'ip' : ip_addr}
        else:
            # ip_reg_ex= r'inet addr:([^.]+.[^.]+.[^.]+.[^\s]+)'
            # ip_found = subprocess.check_output("ifconfig").decode()
            # matches = re.findall(ip_reg_ex, ip_found)

            # # There might be many matches. Only one cared about starts with 192.168 (router perspective)
            # # https://qr.ae/pNs807
            # reduce_matches = lambda match: match.startswith("192.168.")
            # ip_addr =  list(filter(reduce_matches, matches))[0]
            # return ip_addr
            name = socket.gethostname()
            # Forces getting the local IP behind the NAT
            ip_addr = subprocess.check_output("hostname -I", shell=True).decode().strip()
            return {'hostname' : name, 'ip' : ip_addr}