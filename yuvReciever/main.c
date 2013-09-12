#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "jpeg_utils.h"
#include "v4l2uvc.h"
struct stack
{
  char buffer[62000 - 22 - 5*sizeof(int)];
  struct stack* next;
};

typedef struct stack* Stack;
int main()
{
    printf("Lounched\n");
    int sock,listener;
    struct sockaddr_in addr;
    const int sizeOfUDP = 62000;
    char buf[3 * sizeOfUDP + 3 * 21 + 3 * 5 * sizeof(int)];
    int bytes_read = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)  printf("Error in binding\n");
    char camID[5];
    camID[4]='\0';
    char macAddr[18];
    macAddr[17]='\0';
    int IDOfUDP = 0;
    int numberOfPieces = 0;
    int pieceNumber = 0;
    int height = 0;
    int width = 0;
    unsigned char* jpeg = (unsigned char*)malloc(3*sizeOfUDP-3*21-3*5*sizeof(int));
    unsigned char* yuv = (unsigned char*)malloc(153600 - 3 * 21 - 3 * 5 * sizeof(int));

    int size = 0;
    int actualSizeOfYuv = 0;
    int shift = 0;
    struct vdIn info;
    while(1)
    {
        bytes_read = recvfrom(sock, buf,sizeOfUDP + 21 + 5 * sizeof(int), 0, NULL, NULL);  // WARNING! Maybe the size is not correct
        actualSizeOfYuv += bytes_read;
        memcpy(macAddr, buf, 17);
        memcpy(camID, buf + 17, 4);
        memcpy(&IDOfUDP,        buf + 21, sizeof(int));
	memcpy(&numberOfPieces, buf + 21 + 1 * sizeof(int), sizeof(int));
	memcpy(&pieceNumber,    buf + 21 + 2 * sizeof(int), sizeof(int));
	memcpy(&height,         buf + 21 + 3 * sizeof(int), sizeof(int));
	memcpy(&width,          buf + 21 + 4 * sizeof(int), sizeof(int));
        memcpy(yuv+shift,       buf + 21 + 5 * sizeof(int), bytes_read - 21 - 5 * sizeof(int));
        printf("%d piece, bytes have been read %d, the first byte is %x, the last byte is %x, shift is %x\n",pieceNumber, bytes_read,*(yuv + shift),*(yuv + shift + bytes_read - 21 - 5 * sizeof(int) ),shift);
        shift+=bytes_read - 21 - 5 * sizeof(int);
        bytes_read = 0;

        if (pieceNumber == numberOfPieces) 
        {
          FILE* file = fopen("yuv.jpeg","wb");
          //FILE* fileYuv = fopen("yuv","wb");

          printf("Actual size was %d\n",actualSizeOfYuv);
          shift = 0;
          info.framebuffer = yuv;
          info.height = height;
          info.width = width;
	  size = compress_yuyv_to_jpeg(&info,jpeg,actualSizeOfYuv,actualSizeOfYuv);
          //fwrite(yuv,actualSizeOfYuv,1,fileYuv);
          fwrite(jpeg,size,1,file);
          actualSizeOfYuv = 0;
          fclose(file);
          //fclose(fileYuv);
        }
    }


    free(yuv);
    free(jpeg);
    return 0;
}

