server : main.o ../lib/utility.o
	gcc -o server main.o ../lib/utility.o

main.o : main.c
	gcc -c main.c

../lib/utility.o : ../lib/utility.h ../lib/utility.c
	gcc -o ../lib/utility.o -c ../lib/utility.c

clean :
	rm server main.o ../lib/utility.o