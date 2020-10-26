filter:
	clang \
	-fsanitize=signed-integer-overflow \
	-fsanitize=undefined \
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
	-o bin/filter \
	filter.c helpers.c

greycourt:
	./bin/filter -g images/courtyard.bmp out/courtyard.bmp; open out/courtyard.bmp

reflectcourt:
	./bin/filter -r images/courtyard.bmp out/courtyard.bmp; open out/courtyard.bmp

blurcourt:
	./bin/filter -b images/courtyard.bmp out/courtyard.bmp; open out/courtyard.bmp

edgecourt:
	./bin/filter -e images/courtyard.bmp out/courtyard.bmp; open out/courtyard.bmp

test_helpers:
	clang helpers.c test_helpers.c -o bin/test_helpers
	./bin/test_helpers

test: test_helpers
