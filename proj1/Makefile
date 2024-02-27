# Makefile IAN Project 1
# Marek Gergel (xgerge01)

PROG_NAME := ian_proj1
CC := gcc
CCFLAGS := -g -O0 -std=c99 -Wall -Wextra -pedantic
SRC_FILES := $(wildcard *.c)
TESTDATA := $(wildcard testData/*.c)

.PHONY: all $(PROG_NAME) test clean zip tar

all: $(PROG_NAME)

$(PROG_NAME): $(SRC_FILES)
	$(CC) $(CCFLAGS) $(SRC_FILES) -o $@ -lelf

test-compile:
	@for test in $(TESTDATA:%.c=%); do $(CC) $(CCFLAGS) -c $$test.c -o $$test.o; done

test: all test-compile
	@for test in $(TESTDATA:%.c=%); do ./$(PROG_NAME) $$test.o >$$test.out; if diff -q $$test.out $$test.ref >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi done

clean:
	rm -rf $(PROG_NAME) $(PROG_NAME).zip $(PROG_NAME).tar
	rm -rf testData/*.out testData/*.o 

zip:
	zip -r $(PROG_NAME).zip *.h *.c Makefile

tar:
	tar -cf $(PROG_NAME).tar.gz *.h *.c Makefile