#!/bin/bash

DEBUG_BUILD=true
HELP=false

for a in "$@"; do
  case "$a" in
    "--help")         HELP=true         ;;
    "-r"|"--release") DEBUG_BUILD=false ;;
    "-d"|"--debug")   DEBUG_BUILD=true  ;;
    *)
	echo "Unknown option: $a" >&2
	HELP=true
	;;
  esac
done

if $HELP; then
    echo "Usage: $0 [OPTION]...

  --help
	display this message
  -r --release
	compile and build using release flags
  -d --debug (default)
	compile and build using debug flags"
    exit 0
fi

EMSDK_DIR="$HOME/.local/share/emsdk"

if [ ! -f "$EMSDK_DIR/emsdk_env.sh" ]; then
  echo "error: emsdk not found at $EMSDK_DIR" >&2
  exit 1
fi

EMSDK_QUIET=1 source "$EMSDK_DIR/emsdk_env.sh"

EM_HEADER_DIR="$EMSDK_DIR/upstream/emscripten/cache/sysroot/include/"

if [ ! -d "$EM_HEADER_DIR" ]; then
  echo "error: emscripten is not installed properly" >&2
  exit 1
fi

SOURCE="main"
FLAGS="\
	-Wall \
	-Wextra \
	-Werror=vla \
	-std=gnu17 \
	-I $EM_HEADER_DIR"

mkdir -p build ||
    exit 1

emcc -MJ compile_commands.part -c $SOURCE.c $FLAGS &&
    {
	if $DEBUG_BUILD; then
	    FLAGS+=" \
		-g \
		-O0 \
		-sASSERTIONS=2"
	else
	    FLAGS+=" \
		-Os \
		-sASSERTIONS=0 \
		-DNDEBUG"
	fi

	(echo '['; cat compile_commands.part; echo ']') > compile_commands.json &&
	    rm compile_commands.part

	emcc -o build/index.html $SOURCE.o libraylib.a \
	     $FLAGS \
	     --shell-file shell.html \
	     -s USE_GLFW=3 \
	     --preload-file assets \
	     -lm
    }
