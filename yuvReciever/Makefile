all: reciever

reciever: main.c
	gcc main.c -D_REENTERANT v4l2uvc.h jpeg_utils.h jpeg_utils.c stack.c -o reciever -ljpeg -I/usr/include/nptl -L/usr/lib/nptl -lpthread
clean:
	rm -f reciever

