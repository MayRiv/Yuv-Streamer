#ifndef STACK_H
#define STACK_H
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>
struct stack
{
  struct stack* next;
  unsigned char* buffer;
  int size;
  int IDOfFrame;
  int pieceNumber;
  int numberOfPieces;
  int height;
  int width;
  char camID[5];
  char macAddr[18];
};
typedef struct stack Stack;
pthread_mutex_t mutex;
int addPiece(Stack** head, unsigned char* buf, int size, int IDOfFrame, int pieceNumber, int numberOfPieces, int height, int width, char* camID, char* macAddr);
int getFrame(Stack** head, unsigned char* frame, int* height, int* width);
#endif
