part1:  part1.c
	gcc -o part1 part1.c -lpthread

sync:	part1.c
	gcc -o part1 part1.c -lpthread -D PTHREAD_SYNC

clean: 
	rm -f *.o part1 *~ core
