#!/bin/sh
gcc -O3 -g -Wall main_v.c -o main_v.o -lm `sdl2-config --cflags --libs` && ./main_v.o 
