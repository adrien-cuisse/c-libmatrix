
CC=gcc
RELEASE_CFLAGS=-ansi -pedantic -Wall -Wextra -Werror
TESTS_CFLAGS=$(subst -ansi,,$(RELEASE_CFLAGS)) # Criterion is NOT C89-compliant
TESTS_LDFLAGS=-lcriterion

RELEASE_SRC=$(shell find src/ -type f -name '*.c')
RELEASE_OBJ=$(subst src/,obj/,$(RELEASE_SRC:.c=.o))

TESTS_SRC=$(shell find test/src/ -type f -name '*.c')
TESTS_OBJ=$(subst src/,obj/,$(TESTS_SRC:.c=.o))
TESTS_BIN=$(subst src/,bin/,$(TESTS_SRC:.c=))

default: run-tests

obj/%.o: src/%.c
	$(CC) $(RELEASE_CFLAGS) -c $^ -o $@

test/obj/%.o: test/src/%.c
	$(CC) $(TESTS_CFLAGS) -c $^ -o $@

test/bin/%: test/obj/%.o $(RELEASE_OBJ)
	$(CC) $(TESTS_LDFLAGS) $^ -o $@

run-tests: $(TESTS_BIN)
	./$^ || true
#	./$^ --verbose || true

clean:
	rm -f $(RELEASE_OBJ) $(TESTS_OBJ)

clean-all: clean
	rm -f $(TESTS_BIN)
