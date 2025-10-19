#!/bin/bash

set -xe

OS=$(uname -s)

if [ "$OS" = "Linux" ]; then
    gcc main.c include/consts.c include/physics.c include/springs.c -lraylib -lOpenCL -lm -lGL -lpthread -ldl -lrt -lX11 -Wall -Wextra
    
elif [ "$OS" = "Darwin" ]; then
    gcc main.c \
        -lraylib \
        -framework OpenCL \
        -framework OpenGL \
        -framework Cocoa \
        -framework IOKit \
        -framework CoreVideo \
        -lm
        
else
    echo "Unsupported OS: $OS"
    exit 1
fi

./a.out
rm a.out

exit
