all: transfer.c
	gcc -c transfer.c -o transfer.o
	gcc transfer.o -lrbd -lrados -o transfer
clean :
	rm transfer.o transfer
