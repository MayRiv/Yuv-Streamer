#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "jpeg_utils.h"
#include "v4l2uvc.h"
#include "stack.h"
#include <unistd.h>
extern pthread_mutex_t mutex;
Stack* head = NULL;
int convertYuv(unsigned char* input, unsigned char* output, int size);
void* getPieces(void* args);
int main() {
    pthread_mutex_init(&mutex, NULL);
    int sizeOfUDP = 62000;
    unsigned char* jpeg = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));
    unsigned char* yuv  = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));
    unsigned char* yuvListTest  = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));

    int size = 0;
    int actualSizeOfYuv = 0;
    int shift = 0;

    struct vdIn info;
    pthread_t thread;
    pthread_create(&thread, NULL, getPieces, NULL);
    int height = 0;
    int width  = 0;
    sleep(1);
    unsigned char* OlegTestYuv = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));
    while(1)
    {
        usleep(80000);
        //sleep(1);
        int size = getFrame(&head, yuvListTest, &height, &width);
        if (size < 0) continue;
        FILE* fileYuv    = fopen("OlegTestYuv","wb");
        FILE* file       = fopen("yuv.jpeg","wb");
        convertYuv(yuvListTest, OlegTestYuv, size);
//        fwrite(yuvListTest, size, 1, fileYuv);
        fwrite(OlegTestYuv, size, 1, fileYuv);
        info.framebuffer = yuvListTest;//yuv;
        info.height      = height;
        info.width       = width;
        printf("%d %d\n",height, width);
        size             = compress_yuyv_to_jpeg(&info,jpeg, size, size);
        fwrite(jpeg, size, 1, file);
        fclose(fileYuv);
        fclose(file);
    }


    free(yuv);
    free(jpeg);
    return 0;
}


void* getPieces(void* args)
{
  int sock;
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
  unsigned char* yuv  = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));
  int actualSizeOfYuv = 0;
  char camID[5];
  camID[4]='\0';
  char macAddr[18];
  macAddr[17]='\0';
  int IDOfUDP = 0;
  int numberOfPieces = 0;
  int pieceNumber = 0;
  int height = 0;
  int width = 0;
  int shift = 0;
  while(1)
  {
    bytes_read = recvfrom(sock, buf,sizeOfUDP + 21 + 5 * sizeof(int), 0, NULL, NULL);
    actualSizeOfYuv += bytes_read;
    memcpy(macAddr, buf, 17);
    memcpy(camID, buf + 17, 4);
    memcpy(&IDOfUDP,        buf + 21, sizeof(int));
    memcpy(&numberOfPieces, buf + 21 + 1 * sizeof(int), sizeof(int));
    memcpy(&pieceNumber,    buf + 21 + 2 * sizeof(int), sizeof(int));
    memcpy(&height,         buf + 21 + 3 * sizeof(int), sizeof(int));
    memcpy(&width,          buf + 21 + 4 * sizeof(int), sizeof(int));
    memcpy(yuv + shift,     buf + 21 + 5 * sizeof(int), bytes_read - 21 - 5 * sizeof(int));
    addPiece(&head, yuv + shift, bytes_read - 21 -5 * sizeof(int), IDOfUDP, pieceNumber, numberOfPieces, height, width, camID, macAddr);
    //printf("%s %s\n",camID,macAddr);
    shift += bytes_read - 21 - 5 * sizeof(int);
    bytes_read = 0;
    if (pieceNumber == numberOfPieces) shift = 0; 
  }


} 


int convertYuv(unsigned char* input, unsigned char* output, int size)
{
  int i ;
  for ( i = 0; i < size; i += 2 )
    output[ i / 2 ] =  input[i];       // y
  int j = size / 2;
  for ( i = 1; i < size; i += 4 )       // v
    output[ j + i / 4 ] = input[i];
  j = size / 2 + size / 8 ;             // u
  for ( i = 3; i < size; i += 4 )
    output[ j + i / 4 ] = input[i];
  return size / 2 + 2 *size / 4;
}
