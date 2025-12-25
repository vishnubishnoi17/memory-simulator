CC = g++
CFLAGS = -std=c++14 -Wall -Iinclude -g
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/allocator/*.cpp) $(wildcard $(SRCDIR)/buddy/*.cpp) $(wildcard $(SRCDIR)/cache/*.cpp) $(wildcard $(SRCDIR)/virtual_memory/*.cpp)
EXEC = memsim

all: $(EXEC)

$(EXEC): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

test: tests/test_main.cpp $(SOURCES)
	$(CC) $(CFLAGS) -o tests $^

clean:
	rm -f memsim
	rm -rf tests
.PHONY: all test clean
