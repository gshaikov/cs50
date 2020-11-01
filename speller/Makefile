speller:
	clang -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -c -o speller.o speller.c
	clang -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -c -o dictionary.o dictionary.c
	clang -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -o speller speller.o dictionary.o -lm

build:
	clang \
	-ggdb3 \
	-O0 \
	-Qunused-arguments \
	-std=c11 \
	-Wall \
	-Werror \
	-Wextra \
	-Wno-sign-compare \
	-Wno-unused-parameter \
	-Wno-unused-variable \
	-Wshadow \
	-lm \
	-o bin/speller \
	speller.c dictionary.c

run_small: build
	./bin/speller dictionaries/small texts/cat.txt

build_test:
	clang \
	dictionary.c \
	tests/*.c \
	test.c \
	-o ./bin/test

test: build_test
	./bin/test
