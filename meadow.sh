#!/bin/bash
#gcc -c -g include/perlin2d.c -o perlin2d.o -Wall -Wextra
#gcc -c -g include/reference.c -o reference.o -Wall -Wextra
#gcc -c -g main.c -o meadow.o -Wall -Wextra
#
#add -lm if I start using math.h
#gcc -g meadow.o perlin2d.o -o meadow -lncurses -lm
gcc -g main.c -o meadow -lncurses -lm
