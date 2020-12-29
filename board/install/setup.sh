#!/bin/bash
#@file: Used to direct scripts in the process of installing pieces of the C++ code
[[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]] && isWindows=true || isWindows=false

# Check permissions in case of linux
if [[ "${isWindows}" = false ]]; then
    if [ "$EUID" -ne 0 ]; then
        echo "Please run as root ('sudo')"
        exit
    fi
else # is windows
    echo "Install for windows is not supported. Use linux or WSL instead"
    exit
fi

# Flags to control install
print_flags () {
    echo "========================================================================================================================="
    echo "Usage: setup.sh"
    echo "========================================================================================================================="
    echo "Utility to setup the repo"
    echo "========================================================================================================================="
    echo "How to use:"
    echo "  To Start: ./setup.sh [flags]"
    echo "========================================================================================================================="
    echo "Available Flags (mutually exclusive):"
    echo "    -a | --install-all: (Default) Tnstalls everything (recommended for fresh installs)"
    echo "    -p | --linux-pcks: Install all the required linux packages"
    echo "    -s | --submodules: Fetch & Update all the git submodules in this repo"
    echo "    -g | --gpio: Build and Install the c++ gpio library from source"
    echo "    -h | --help: This message"
    echo "========================================================================================================================="
}

# parse command line args
install_gpio=false
update_submodules=false
install_pcks=false
install_all=true # default to installing everything
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -a | --install-all )
            echo "Installing all externals!"
            break
            ;;
        -p | --linux-pcks )
            install_pcks=true
            install_all=false
            break
            ;;
        -g | --gpio )
            install_gpio=true
            install_all=false
            break
            ;;
        -s | --submodules )
            update_submodules=true
            install_all=false
            break
            ;;
        -h | --help )
            print_flags
            exit 0
            ;;
        * )
            echo "... Unrecognized Command: $1"
            print_flags
            exit 1
    esac
    shift
done


# Get absolute paths for the project
INSTALL_DIR="$(readlink -fm "$0"/..)"
root_dir="$(readlink -fm "${INSTALL_DIR}/..")" # board/ dir. Just for C++, exludes rest of project and Python
extern_dir="${root_dir}/extern"
helpers_dir="${INSTALL_DIR}/helpers"

# Get helper script paths
wiring_pi_script="${helpers_dir}/wiring_pi.sh"
submodule_script="${helpers_dir}/submodules.sh"
linux_pcks_script="${helpers_dir}/linux_pcks.sh"

# Call the helpers in the right order
echo "------------Calling Install Helper Scripts------------"

# linux packages
if [[ ${install_pcks} == true || ${install_all} == true ]]; then
    bash "${linux_pcks_script}"
fi


# script to get git submodules
if [[ ${update_submodules} == true || ${install_all} == true ]]; then
    bash "${submodule_script}" \
        --root-dir "${root_dir}"
fi

# external scripts
if [[ ${install_gpio} == true || ${install_all} == true ]]; then
    bash "${wiring_pi_script}" \
        --extern-dir "${extern_dir}"
fi

echo "------------Completed Scripts for Install------------"
echo "Overall Install / Setup complete!!!!"