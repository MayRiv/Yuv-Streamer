all: reciever reciever-DOZOR

reciever: main.c
	gcc main.c -D_REENTERANT  stack.c -o reciever  -I/usr/include/nptl -L/usr/lib/nptl -lpthread
reciever-DOZOR: main.c
	arm-linux-gcc main.c -D_REENTERANT  stack.c -o reciever-DOZOR  -I/usr/include/nptl -L/usr/lib/nptl -lpthread        
clean:
	rm -f reciever reciever-DOZOR

