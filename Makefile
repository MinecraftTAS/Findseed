TARGET = findseed

CC = gcc
CFLAGS = -Wno-unused-parameter -Wall -Wextra -Werror -std=gnu17 -pedantic
LDFLAGS = -pthread -ldiscord -lcurl

ifndef PROD
CFLAGS += -g
else
CFLAGS += -O3 -march=native -mtune=native
LDLAGS += -flto=auto
endif

.ONESHELL:

build/$(TARGET): src/$(TARGET).c
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

all: build/$(TARGET)

run: build/$(TARGET)
	cd build
	./$(TARGET)

debug: build/$(TARGET)
	cd build
	gdb -q ./$(TARGET)

leaks: build/$(TARGET)
	cd build
	valgrind --leak-check=full  ./$(TARGET)

clean:
	rm -f build/$(TARGET) build/*.log

.PHONY: all run debug clean
