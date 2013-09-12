all: streamer

streamer: main.c
	gcc main.c v4l2uvc.h jpeg_utils.h jpeg_utils.c -o streamer -ljpeg
clean:
	rm -f streamer

