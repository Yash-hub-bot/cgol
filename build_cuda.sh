#!/bin/sh
nvcc -g  main.cu -o main.co `sdl2-config --cflags --libs` && ./main.co
