all: reciever reciever-DOZOR

reciever: main.c
	gcc main.c -D_REENTERANT v4l2uvc.h jpeg_utils.h jpeg_utils.c stack.c -o reciever -ljpeg -I/usr/include/nptl -L/usr/lib/nptl -lpthread
reciever-DOZOR: main.c
	arm-linux-gcc main.c -D_REENTERANT v4l2uvc.h jpeg_utils.h jpeg_utils.c stack.c -o reciever-DOZOR -ljpeg -I/usr/include/nptl -L/usr/lib/nptl -lpthread        
clean:
	rm -f reciever

