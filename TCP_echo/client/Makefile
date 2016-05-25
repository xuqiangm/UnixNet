client : main.o ../lib/algorithm.o
	gcc -o client main.o ../lib/algorithm.o

main.o : main.c
	gcc -c main.c

../lib/algorithm.o : ../lib/algorithm.h ../lib/algorithm.c
	gcc -o ../lib/algorithm.o -c ../lib/algorithm.c

clean :
	rm client main.o ../lib/algorithm.o