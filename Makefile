CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -Werror -O2
SRC = src/test_basic.c src/pcg.c
OBJ = $(SRC:.c=.o)
EXEC = rng_pcg_test

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build complete: $(EXEC)"	

clean:
	rm -f $(OBJ) $(EXEC)
	@echo "Cleaned up."

.PHONY: clean	