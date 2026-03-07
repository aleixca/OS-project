CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o terminal.o commands.o io.o maester.o inventory.o signal.o trade.o

maester: $(OBJ)
	$(CC) $(CFLAGS) -o maester $(OBJ)

main.o: main.c terminal.h inventory.h maester.h signal.h
	$(CC) $(CFLAGS) -c main.c

terminal.o: terminal.c terminal.h commands.h io.h maester.h inventory.h trade.h signal.h
	$(CC) $(CFLAGS) -c terminal.c

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c commands.c

io.o: io.c io.h
	$(CC) $(CFLAGS) -c io.c

maester.o: maester.c maester.h io.h
	$(CC) $(CFLAGS) -c maester.c

inventory.o: inventory.c inventory.h io.h
	$(CC) $(CFLAGS) -c inventory.c

signal.o: signal.c signal.h
	$(CC) $(CFLAGS) -c signal.c

trade.o: trade.c trade.h io.h
	$(CC) $(CFLAGS) -c trade.c

clean:
	rm -f *.o maester

re: clean maester
