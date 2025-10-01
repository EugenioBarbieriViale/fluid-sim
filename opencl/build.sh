#!/bin/bash

# set -xe

gcc -o main main.c -lOpenCL -Wall

./main

# rm main

exit
