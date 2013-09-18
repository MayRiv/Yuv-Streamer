#include "stack.h"
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int length(Stack *head);
int addPiece(Stack** head, unsigned char* buf, int size, int IDOfFrame, int pieceNumber, int numberOfPieces, int height, int width, char* camID, char* macAddr)
{
  pthread_mutex_lock(&mutex);
  Stack* headp = *head;

  Stack* newPiece = (Stack*)malloc(sizeof(Stack));
  newPiece->IDOfFrame                  = IDOfFrame;
  newPiece->size                       = size;
  newPiece->buffer                     = (unsigned char*)malloc(size);
  newPiece->pieceNumber                = pieceNumber;
  newPiece->numberOfPieces             = numberOfPieces;
  newPiece->height                     = height;
  newPiece->width                      = width;
  memcpy(newPiece->buffer, buf, size);
  memcpy(newPiece->camID, camID, 4);
  memcpy(newPiece->macAddr, macAddr, 17);

  Stack* parent = NULL;
  while(headp && (pieceNumber > headp->pieceNumber || headp->IDOfFrame != IDOfFrame))
  {
   parent = headp;
   headp = headp->next;
  }
  newPiece->next = headp;
  if (!parent) *head = newPiece;
  else parent->next = newPiece;
  pthread_mutex_unlock(&mutex);
}
int getFrame(Stack** head, unsigned char* frame, int* height, int* width)
{
  if (!*head) return -1;
  pthread_mutex_lock(&mutex);
  Stack* headp          = *head;
  int piece             = 0;
  int size              = 0;
  int shift             = 0;
  int IDOfFrame         = (*head)->IDOfFrame;
  int totalSize         = 0;
  printf("The length is %d\n", length(*head));
  while( headp != 0)
  {
    memcpy(frame + shift, headp->buffer, headp->size);
    totalSize      += headp->size;
    shift          += headp->size;
    Stack* toDelete = headp;
    if (headp->pieceNumber == headp->numberOfPieces) 
    {
      printf("IDOfFrame is %d\n",headp->IDOfFrame);
      *height = headp->height;
      *width  = headp->width;
      *head   = headp->next;
      free(toDelete);
      pthread_mutex_unlock(&mutex);
      return totalSize;
    }
    if ((headp->next == 0) || (headp->IDOfFrame != headp->next->IDOfFrame)){pthread_mutex_unlock(&mutex); return -1;}

    headp = headp->next;
    free(toDelete);
  }
  memset(frame, 0, totalSize);
  printf("There is no entire frame! The last piece is %d\n", headp->pieceNumber);
  pthread_mutex_unlock(&mutex);  
  return -1;
}
int length(Stack* head)
{
  int len = 0;
  while(head)
  {
    len++;
    head = head->next;
  }
  return len;

}
