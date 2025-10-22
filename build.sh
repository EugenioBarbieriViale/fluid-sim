#!/bin/bash

set -xe

OS=$(uname -s)

CFILES="src/main.c src/host_types.c src/host.c"
CFLAGS="-I./include -lm -Wall -Wextra"

if [ "$OS" = "Linux" ]; then
    gcc $CFILES $CFLAGS -lraylib -lOpenCL -lGL -lpthread -ldl -lrt -lX11
    
elif [ "$OS" = "Darwin" ]; then
    gcc $CFILES $CFLAGS \
        -lraylib \
        -framework OpenCL \
        -framework OpenGL \
        -framework Cocoa \
        -framework IOKit \
        -framework CoreVideo \
        
else
    echo "Unsupported OS: $OS"
    exit 1
fi

./a.out
rm a.out

exit
