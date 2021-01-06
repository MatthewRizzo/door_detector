server_dir_path="$(readlink -fm $0/..)"
venv_path=$server_dir_path/venv
python_path=$venv_path/bin/python

# Run the script using the flags given. if -h is passed internal CLI handles it
$python_path main.py "$@"