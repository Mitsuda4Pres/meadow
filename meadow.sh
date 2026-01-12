#!/bin/bash
#gcc -c -g include/m4pcm.c -o m4pcm.o -Wall -Wextra
#gcc -c -g include/reference.c -o reference.o -Wall -Wextra
#gcc -c -g pcmparser.c -o pcmparser.o -Wall -Wextra
#
#add -lm if I start using math.h
gcc -g main.c -o meadow -lncurses
