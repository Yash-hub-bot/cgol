#!/bin/sh
gcc -g -Wall main.c -o main.o `sdl2-config --cflags --libs` && ./main.o 
