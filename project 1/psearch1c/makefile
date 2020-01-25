CC=gcc

psearch1c: psearch1c.o
	$(CC) -pthread psearch1c.o -o psearch1c

main.o: psearch1c.c
	$(CC) -c psearch1c.c

clean:
	rm *.o output.txt psearch1c