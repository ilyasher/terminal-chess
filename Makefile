CC=gcc
CCFLAGS= # -Wall -Werror

chess: chess.c
	$(CC) $(CCFLAGS) chess.c -o chess -lncurses -lm -std=gnu99

clean:
	rm -f *.o chess
#
# CXX=g++
# CXXFLAGS= -std=c++17 #-Wall -Werror
# POSITION_OBJS = position.o
# ALL_OBJS = position.o rulebook.o ui.o
#
# all: checkers
#
# run: checkers
# 	./checkers
#
# checkers: $(ALL_OBJS)
# 	$(CXX) $(CXXFLAGS) $^ -o $@
#
# position: $(POSITION_OBJS)
# 	$(CXX) $(CXXFLAGS) $^ -o $@
#
# doc: Doxyfile
# 	doxygen
#
# clean:
# 	rm -f position checkers *.o
# 	rm -rf docs
#
# .PHONY: all clean doc run
