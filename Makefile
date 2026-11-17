CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -I$(INC_DIR)
LDFLAGS = -fsanitize=address,undefined
INC_DIR = includes

SRCS = $(wildcard src/*.c)
OBJ = $(SRCS:.c=.o)
TARGET = httpserver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
