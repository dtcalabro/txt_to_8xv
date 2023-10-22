CC = gcc
CFLAGS = -Wall -Wextra -pedantic

main: main.c
	$(CC) $(CFLAGS) -o txt_to_8xv main.c