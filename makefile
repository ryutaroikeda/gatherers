DEBUGGER=valgrind
CFLAGS=-g -O2 -Wextra -Wall -pedantic -Isrc $(OPTFLAGS)
LIBS=$(OPTLIBS)

SOURCES:=$(wildcard src/*.c)
OBJECTS:=$(patsubst %.c,%.o,$(SOURCES))
DEPENDENCIES:=$(patsubst %.c,%.d,$(SOURCES))

TESTSOURCE:=$(wildcard tests/*_tests.c)
TESTOBJECT:=$(patsubst %.c,%.o,$(TESTSOURCE))
TESTS:=$(patsubst %.c,%,$(TESTSOURCE))
DEPENDENCIES+=$(patsubst %.c,%.d,$(TESTSOURCE))

TARGET=bin/gatherers
TARGETMAIN=src/main.o
TESTSCRIPT=tests/runtests.sh

all: $(DEPENDENCIES) $(TARGET) tests

dev: CFLAGS=-g -Wextra -Wall -pedantic -Werror -Isrc
dev: all

$(TARGET): build $(OBJECTS)
	$(CC) $(LIBS) -o $@ $(OBJECTS)

build:
	@mkdir -p bin
	@mkdir -p build

src/%.d: src/%.c
	$(CC) $(CFLAGS) -MM $< -MF $@
	sed -e 's:$*.o:src/$*.o:g' $@ > tmp
	mv -f tmp $@

.PHONY: clean tests

$(TESTS): % : %.o
	$(CC) $(LIBS) -o $@ $< $(subst $(TARGETMAIN),,$(OBJECTS))

tests: $(TESTS)
	sh ./$(TESTSCRIPT)

tests/%.d: tests/%.c
	$(CC) $(CFLAGS) -MM $< -MF $@
	sed -e 's:$*.o:tests/$*.o:g' $@ > tmp
	mv -f tmp $@

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

clean:
	rm -rf build $(OBJECTS) $(TESTS) $(DEPENDENCIES)
	rm -f tests/tests.log

ifeq (,$(filter $(MAKECMDGOALS),clean))
-include $(DEPENDENCIES)
endif
