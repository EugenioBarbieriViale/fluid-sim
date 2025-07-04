#!/bin/bash

set -xe

gcc perfect_gas.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

./a.out

rm a.out

exit
