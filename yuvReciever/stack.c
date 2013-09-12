#include "stack.h"
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int addPiece(Stack** head, unsigned char* buf, int size, int IDOfFrame, int pieceNumber, int numberOfPieces, int height, int width, char* camID, char* macAddr)
{
  pthread_mutex_lock(&mutex);
  Stack** headp = head;
  while(headp && (*headp))    headp = &(*headp)->next;

  if (!(*headp))
  {
    (*headp)                            = (Stack*)malloc(sizeof(Stack));
    (*headp)->next                      = NULL;
    (*headp)->IDOfFrame                 = IDOfFrame;
    (*headp)->size                      = size;
    (*headp)->buffer                    = (unsigned char*)malloc(size);
    (*headp)->pieceNumber               = pieceNumber;
    (*headp)->numberOfPieces            = numberOfPieces;
    (*headp)->height                    = height;
    (*headp)->width                     = width;
    memcpy((*headp)->buffer, buf, size);
    memcpy((*headp)->camID, camID, 4);
    memcpy((*headp)->macAddr, macAddr, 17);
  }
  pthread_mutex_unlock(&mutex);
}
int getFrame(Stack** head, unsigned char* frame)
{
  pthread_mutex_lock(&mutex);
  Stack** headp = NULL;
  if (head) headp = head;
  else printf("Stack is empty\n");
  int piece                       = 0;
  int size                        = 0;
  int shift                       = 0;
  int IDOfFrame                   = (*headp)->IDOfFrame;
  int totalSize                   = 0;
  if ((*headp)->pieceNumber != 0)  printf("Not a first piece\n");
  while(headp)
  {
    printf("%d piece, %d numberOfPieces\n", (*headp)->pieceNumber, (*headp)->numberOfPieces);
    memcpy(frame + shift, (*headp)->buffer, (*headp)->size);
    shift     += (*headp)->size;
    totalSize += (*headp)->size;
    if ((*headp)->pieceNumber == (*headp)->numberOfPieces)
    {
      pthread_mutex_unlock(&mutex);
      return totalSize;
    }
    if ((*headp)->IDOfFrame == (*headp)->next->IDOfFrame)
    {
      Stack* temp = *headp;
      headp = &(*headp)->next;
      //free(temp);  deleting this causes seg fault.
      printf("test\n");
    }
    else break;
  }
  memset(frame, 0, totalSize);
  printf("There is no entire frame! The last piece is %d\n", (*headp)->pieceNumber);
  pthread_mutex_unlock(&mutex);  
  return -1;

}
