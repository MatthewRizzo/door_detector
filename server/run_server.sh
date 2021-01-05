server_dir_path="$(readlink -fm $0/..)"
venv_path=$server_dir_path/venv
python_path=$venv_path/bin/python

$python_path main.py