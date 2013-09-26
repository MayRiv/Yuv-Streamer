#include "stack.h"
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int length(Stack* head);
void showStack(Stack* head);
int addPiece(Stack** head, unsigned char* buf, int size, int IDOfFrame, int pieceNumber, int numberOfPieces, int height, int width, char* camID, char* macAddr)
{
  pthread_mutex_lock(&mutex);
  if ((length(*head) != 0) && (length(*head) > 4 * (*head)->numberOfPieces))
  {
    printf("There too many elements in the stack. Aborted\n");
    pthread_mutex_unlock(&mutex);
    return -1;
  }
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
  pthread_mutex_lock(&mutex);
  if (!*head || length(*head) < 3) 
  {
    pthread_mutex_unlock(&mutex);
    return -1;
  }
  //showStack(*head);
  Stack* headp          = *head;
  int piece             = 0;
  int size              = 0;
  int shift             = 0;
  int IDOfFrame         = (*head)->IDOfFrame;
  int totalSize         = 0;
  Stack** arrayToDelete = (Stack**)malloc(sizeof(Stack*) * (headp->numberOfPieces + 1));
  memset(arrayToDelete, 0, (headp->numberOfPieces+1) * sizeof(Stack*));
  int counter           = 0;
  while( headp != 0)
  {
    memcpy(frame + shift, headp->buffer, headp->size);
    totalSize      += headp->size;
    shift          += headp->size;
    arrayToDelete[counter] = headp;
    counter++;
    if (counter == (headp->numberOfPieces + 1) && headp->pieceNumber == headp->numberOfPieces)   //there were last and first pieces of the frame.
    {
      int i=0;
      *head   = headp->next;
      *height = headp->height;
      *width  = headp->width;

      for (i = 0; i < headp->numberOfPieces + 1; i++)    
      {
        if (arrayToDelete[i]) free(arrayToDelete[i]->buffer);
        free(arrayToDelete[i]);
      }
      free(arrayToDelete);
      pthread_mutex_unlock(&mutex);
      return totalSize;
    }
    if ((headp->next == 0) || (headp->IDOfFrame != headp->next->IDOfFrame)){ break; }//pthread_mutex_unlock(&mutex); return -1;}
    headp = headp->next;
  }
  int i;
  if (length(*head) > (*head)->numberOfPieces)
  {
    *head = headp->next;
    for (i = 0; i < headp->numberOfPieces + 1; i++)    
    {
      if (arrayToDelete[i]) free(arrayToDelete[i]->buffer);
      free(arrayToDelete[i]);
    }
  }
  free(arrayToDelete);
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
void showStack(Stack* head)
{
  while(head)
  {
    printf("IDOfFrame is %d, pieceNumber is %d\n", head->IDOfFrame, head->pieceNumber);
    head = head->next;
  }
  printf("\n\n\n");
}
