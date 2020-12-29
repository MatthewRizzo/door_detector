# scp's file to board relative to the board dir in the repo
# Input = the name of the file to transfer

ROOT_PROJECT_DIR="$(readlink -fm "$0"/..)"

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
    echo "    -e | --extern:        Use this for files relative to extern"
    echo "    -i | --install:       Use this for files relative to install"
    echo "    -s | --src:           Use this for files relative to src"
    echo "    -f | --filename:      Name of file to transfer"
    echo "    -h | --help:          This message. Note that the options are in cascading precedence if multiple are picked"
    echo "========================================================================================================================="
}

# parse command line args
relative_dir=""
file_name=""
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -e | --extern )
            relative_dir="extern"
            ;;
        -i | --install )
            relative_dir="install"
            ;;
        -s | --src )
            relative_dir="src"
            ;;
        -f | --filename )
            file_name="$2"
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

if [ "${file_name}" = "" ]; then
    echo "No filename give to -f flag. Please try again."
    exit
fi

if [ "${relative_dir}" = "" ]; then
    echo "No relative directory selected. Please try again. Use -h to print valid flags."
    exit
fi


scp $ROOT_PROJECT_DIR/board/$relative_dir/$file_name pi:~/sandboxes/door_detector/board/$relative_dir/$file_name
