#!/bin/bash
#@file: Builds and Installs externals

# CLI Flags
print_flags () {
    echo "========================================================================================================================="
    echo "Usage: wiring_pi.sh"
    echo "========================================================================================================================="
    echo "Helper utility to setup external libraries for C++ code"
    echo "========================================================================================================================="
    echo "How to use:"
    echo "  To Start: ./wiring_pi.sh [flags]"
    echo "========================================================================================================================="
    echo "Available Flags:"
    echo "    -i | --extern-dir: Path to the extern dir (defaults to git pinned path)"
    echo "    -m | --mode: [install, clean, uninstall]"
    echo "    -h | --help: This message"
    echo "========================================================================================================================="
}

# parse command line args
mode=""
extern_dir="$(readlink -fm "$0"/../../extern/)" # default to git pinned path
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -i | --extern-dir )
            extern_dir="$2"
            shift
            ;;
        -m | --mode )
            mode="$2"
            if [[ ${mode} == "install" ]]; then
                # use ./build "" to do standard builds
                mode=""
            elif [[ ${mode} != "clean" && ${mode} != "uninstall" ]]; then
                echo "Allowed modes are [install, clean, uninstall]"
                exit 1
            fi
            shift
            ;;
        -h | --help )
            print_flags
            exit 0
            ;;
        * )
            echo "Commands note recognized: $1"
            print_flags
            exit 1
    esac
    shift
done

# Determine paths to the libraries
gpio_lib="${extern_dir}/wiring_pi"
gpio_build_binary="${gpio_lib}/build"

# Build and install the libraries

# install the library for gpio. It must be in the correct dir for this (because of chained commands)
echo "------------Running Build and Install for GPIO Library------------"
echo "gpio_lib = ${gpio_lib}"
cd "${gpio_lib}" && \
    bash "${gpio_build_binary}" "${mode}"