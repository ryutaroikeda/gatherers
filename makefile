DEBUGGER=valgrind
CFLAGS=-g -O2 -Wextra -Wall -pedantic -Isrc $(OPTFLAGS)
LIBS=$(OPTLIBS)

SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SOURCE=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SOURCE))

TARGET=bin/gatherers

all: $(TARGET) tests

dev: CFLAGS=-g -Wextra -Wall -pedantic -Werror -Isrc
dev: all

$(TARGET): build $(OBJECTS)
	$(CC) $(LIBS) -o $@ $(OBJECTS)

build:
	@mkdir -p bin
	@mkdir -p build

.PHONY: clean tests

tests: $(TESTS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
