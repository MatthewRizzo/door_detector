#!/bin/bash
[[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]] && is_windows=true || is_windows=false

# if linux, need to check if using correct permissions
if [[ "${is_windows}" = false ]]; then
    if [ "$EUID" -ne 0 ]; then
        echo "Please run as root ('sudo')"
        exit
    fi
fi


# Store the start loc to go back to it when done
install_dir_path="$(readlink -fm $0/..)"
project_root_dir="$(readlink -fm ${install_dir_path}/..)"
venv_path=${project_root_dir}/venv
venv_bin_part=$venv_path/bin
python_version=3.7
python_name=python${python_version}
python_loc="" # Keep it blank and local to be filled in later

# Go to project root - navigate to the install folder and backup one to get to project root
cd $project_root_dir

# Determine the OS -> controls flow of script after this
echo "Step 1: Creating Virtual Environment for the operating system"

echo "Step 1.0.1: Setting up ability to create virtual environment"
python -m pip install virtualenv -q # make it silent

if [[ "${is_windows}" = false ]]; then
    Download python3.7
    echo "Step 1.1: Add the ppa for python"
    add-apt-repository -y ppa:deadsnakes/ppa
    apt update -y
    apt upgrade -y
    apt install -y \
        ${python_name} \
        ${python_name}-venv

    echo "Step 1.2: Setup the Linux Venv"
    echo "Step 1.2.1: Delete any previous  Linux Venv's"
    echo Deleting any existing virtual environment
    rm -rf $venv_path

    echo "Step 1.2.1: Create a new Linux Venv"
    $python_name -m venv $venv_path

    # This needs to be what the python executable in the venv is called
    pip_script=pip3

    venv_bin_path=$venv_path/bin
    cd $venv_bin_path

    # Just in case, update pip
    echo "Step 1.2.3: Upgrading the venv's pip"
    $venv_bin_path/$pip_script install --upgrade pip

    # Install the modules (with their correct versions) to the venv
    echo "Step 1.2.4: Installing the required modules"
    $venv_bin_path/$pip_script install -r $project_root_dir/install/requirements.txt

    python_loc=${venv_bin_path}/python3

    echo "Done setting up virtual environment"
else
    echo "Step 1.1: Checking Python Version. Assumes python = python3"
    current_version_txt=$(python --version)
    current_minor_version=$(echo "$current_version_txt" | awk '{print $2}')
    current_version=$(echo "${current_minor_version}" | sed -r 's/\.[0-9]$//') # Takes away minor version (3.7.2 -> 3.7)

    echo "Using python${current_version} to build virtual environment"
    if [[ ${python_version} != ${current_version} ]]; then
        echo "WARNING: Wrong version of python being used. Expects python${python_version}. This might affect results"
    fi

    echo "Step 1.2: Deleting any existing virtual environment"
    rm -rf ${venv_path}

    echo "Step 1.3: Initializing the Virtual Environment for Windows"
    python -m venv ${venv_path}

    echo "Step 1.4: Installing packages for the Windows Virtual Environment"
    venv_scripts_path=${venv_path}/Scripts/
    python_loc=${venv_scripts_path}/python.exe
    ${python_loc} -m pip install --upgrade pip
    ${python_loc} -m pip install -r ${install_dir_path}/requirements.txt

    echo "Done setting up virtual environment"

fi