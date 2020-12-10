#!/bin/bash
#@file: downloads and updates (as needed) the submodules of this code base

# CLI Flags
print_flags () {
    echo "========================================================================================================================="
    echo "Usage: submodules.sh"
    echo "========================================================================================================================="
    echo "Helper utility to setup/update git submodules in this repo"
    echo "========================================================================================================================="
    echo "How to use:"
    echo "  To Start: ./submodules.sh [flags]"
    echo "========================================================================================================================="
    echo "Available Flags:"
    echo "    --root-dir <dir>: Absolute path to the root of the repo"
    echo "    -h | --help: This message"
    echo "========================================================================================================================="
}


# process command line flags
root_dir=""
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --root-dir )
            root_dir="$2"
            shift
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

# Handle submodules as user (sudo optimal but git and sudo do not cooperate)
echo "------------Updating Git Submodules------------"
cd "${root_dir}" || echo "Failed to cd to repo root: ${root_dir}" && \
    (sudo -u "${SUDO_USER}" git submodule update --init --recursive)