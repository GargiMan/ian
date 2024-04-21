# Makefile IAN Project 1
# Marek Gergel (xgerge01)

PROG_NAME := ian-proj1
CC := gcc
CCFLAGS := -g -O0 -std=c99 -Wall -Wextra -pedantic
TESTDATA := $(wildcard testData/*.c)

.PHONY: all $(PROG_NAME) test test-compile clean zip tar

all: $(PROG_NAME)

$(PROG_NAME): $(PROG_NAME).c
	$(CC) $(CCFLAGS) $^ -o $@ -lelf

test-compile: $(TESTDATA)
	@for test in $(TESTDATA:%.c=%); do $(CC) $(CCFLAGS) -c $$test".c" -o $$test".o"; done
	@for test in $(TESTDATA:%.c=%); do $(CC) $(CCFLAGS) -m32 -c $$test".c" -o $$test"32.o"; done
	@for test in $(TESTDATA:%.c=%); do $(CC) $(CCFLAGS) $$test".c" -o $$test""; done
	@for test in $(TESTDATA:%.c=%); do $(CC) $(CCFLAGS) -m32 $$test".c" -o $$test"32"; done

test: all test-compile
	@for test in $(TESTDATA:%.c=%); do ./$(PROG_NAME) $$test".o" >$$test".out"; if diff -q $$test".out" $$test".ref" >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi; ./$(PROG_NAME) $$test"32.o" >$$test".out"; if diff -q $$test".out" $$test".ref" >/dev/null; then echo "Test OK : $$test (32-bit)"; else echo "Test FAIL : $$test (32-bit)"; fi done
	@for test in $(TESTDATA:%.c=%); do ./$(PROG_NAME) $$test >$$test".out"; if diff -q $$test".out" $$test".ref" >/dev/null; then echo "Test OK : $$test"; else echo "Test FAIL : $$test"; fi; ./$(PROG_NAME) $$test"32" >$$test".out"; if diff -q $$test".out" $$test".ref" >/dev/null; then echo "Test OK : $$test (32-bit)"; else echo "Test FAIL : $$test (32-bit)"; fi done

clean:
	rm -rf $(PROG_NAME) $(PROG_NAME).zip $(PROG_NAME).tar.gz
	rm -rf $(TESTDATA:%.c=%) $(TESTDATA:%.c=%.o) $(TESTDATA:%.c=%32) $(TESTDATA:%.c=%32.o) $(TESTDATA:%.c=%.out) 

zip:
	zip -qr $(PROG_NAME).zip *.c Makefile testData

tar:
	tar -czf $(PROG_NAME).tar.gz *.c Makefile testData
