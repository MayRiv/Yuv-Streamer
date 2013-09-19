#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include <unistd.h>
extern pthread_mutex_t mutex;
Stack* head = NULL;
const int sizeOfUDP = 62000;
int convertYuv(unsigned char* input, unsigned char* output, int size, int height, int width);
void* getPieces(void* args);
int main(int argc, char** argv) {
    pthread_mutex_init(&mutex, NULL);
    unsigned char* convertedYuv = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));
    unsigned char* yuv  = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));

    int size = 0;
    int actualSizeOfYuv = 0;
    int shift = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, getPieces, NULL);
    int height = 0;
    int width  = 0;
    char* fileName = NULL;
    if (argc < 2) fileName = "OlegTestYuv";
    else fileName = argv[1];
    while(1)
    {
        usleep(20000);
        //sleep(1);
        int size      = getFrame(&head, yuv, &height, &width);
        if (size < 0) continue;
        FILE* fileYuv = fopen(fileName,"wb");
        int sizeOfConvertedYuv = convertYuv(yuv, convertedYuv, size, height, width);
        fwrite(convertedYuv,  sizeOfConvertedYuv, 1, fileYuv);
        printf("%d %d\n",height, width);
        fclose(fileYuv);
    }
    free(yuv);
    free(convertedYuv);
    return 0;
}


void* getPieces(void* args)
{
  int sock;
  struct sockaddr_in addr;
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
  unsigned char* yuv  = (unsigned char*)malloc(3 * sizeOfUDP - 3 * 21 - 3 * 5 * sizeof(int));

  while(1)
  {
    bytes_read = recvfrom(sock, buf, sizeOfUDP + 21 + 5 * sizeof(int), 0, NULL, NULL);
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
    //shift += bytes_read - 21 - 5 * sizeof(int);
    bytes_read = 0;
    if (pieceNumber == numberOfPieces) shift = 0; 
  }
    free(yuv);
}


int convertYuv(unsigned char* input, unsigned char* output, int size, int heigth, int width)
{
  int i = 0 ;
  for (i = 0; i < size / 2; i++)
    output[i] =  input[i * 2];                                                                  // y
  int shiftInOutput = size / 2;
  
  int row = 0;
  int col = 0;
  for (col = 0; col < heigth; col += 2)
    for (row = 1; row < 2 * width; row +=4 ) output[shiftInOutput++] = input[col * heigth + row];  //u
  for (col = 0; col < heigth; col += 2)
    for (row = 3; row < 2 * width; row +=4 ) output[shiftInOutput++] = input[col * heigth + row];  //v
  return  shiftInOutput;
}
