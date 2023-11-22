COMPILER=gcc
OPTIONS=-std=c17 -Wall
OPTIONS_DEBUG=-ggdb $(OPTIONS)
COMPILE=$(COMPILER) $(OPTIONS)
COMPILE_DEBUG=$(COMPILER) $(OPTIONS_DEBUG)

all: hanoi

hanoi: main.c solver.c
	$(COMPILE_DEBUG) -o $@ $^ -lncurses

run: hanoi
	./$^

clean:
	rm hanoi
