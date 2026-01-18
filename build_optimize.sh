#!/bin/sh
gcc -O3 -g -Wall main_optimize.c -o main_o.o -lm `sdl2-config --cflags --libs` && ./main_o.o 
