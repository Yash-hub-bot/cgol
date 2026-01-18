#!/bin/sh
gcc -O3 -g -Wall main.c -o main.o -lm `sdl2-config --cflags --libs` && ./main.o 
