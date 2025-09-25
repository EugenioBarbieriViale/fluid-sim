#!/bin/bash

set -xe

gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall

./a.out

rm a.out

exit
