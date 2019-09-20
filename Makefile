#
# Makefile
#
# CS50 AP
# Sudoku
#

sudoku: Makefile chess.c
	gcc -ggdb -std=c99 -Wall -Werror -Wno-unused-but-set-variable -o chess chess.c -lncurses -lm -std=gnu99

clean:
	rm -f *.o a.out core log.txt chess
