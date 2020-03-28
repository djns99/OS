all: tests

ROOT = ..
SRC = $(ROOT)/src
INCLUDE = $(ROOT)/src

MemoryTest: MemoryTest.c
	gcc -g -ggdb -O0 -D TEST_MODE=1 -Wall MemoryTest.c $(SRC)/memory.c -o $@ -I$(INCLUDE)

TESTS = MemoryTest

.PHONY: run
run: 
	@for t in $(TESTS); do echo "./$$t" | bash; done

.PHONY: tests $(TESTS)
tests: $(TESTS)
	@echo "======== Running Tests ==========="
	-@for t in $(TESTS); do echo "./$$t" | bash; done
	@echo "======== Tests Complete ==========="

clean:
	rm -f $(TESTS)

