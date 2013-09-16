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
  /*
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
  */
  Stack* headp = *head;
 /* if (headp == NULL)
  {
    printf("Stack is empty\n");
    return 0;
  }
*/
  Stack* newPiece = (Stack*)malloc(sizeof(Stack));
  newPiece->IDOfFrame                  = IDOfFrame;
  newPiece->size                       = size;
  newPiece->buffer                     = (unsigned char*)malloc(size);
  newPiece->pieceNumber                = pieceNumber;
  newPiece->numberOfPieces             = numberOfPieces;
  printf("Number of pieces is %d, piece number is %d\n",newPiece->numberOfPieces, newPiece->pieceNumber); 
  newPiece->height                     = height;
  newPiece->width                      = width;
  memcpy(newPiece->buffer, buf, size);
  memcpy(newPiece->camID, camID, 4);
  memcpy(newPiece->macAddr, macAddr, 17);

  Stack* parent = NULL;
//  while(headp && pieceNumber < headp->pieceNumber)   headp = headp->next;
  while(headp && (pieceNumber > headp->pieceNumber || headp->IDOfFrame != IDOfFrame))
  {
   parent = headp;
   headp = headp->next;
  }
  /*if (!headp->next)
  {
    newPiece->next = NULL;
    headp->next = newPiece;
  }
  else if (headp)
  {
    newPiece->next = headp->next;
    headp->next = newPiece;
  }
  else newPiece->next = NULL;*/
  newPiece->next = headp;
  if (!parent) *head = newPiece;
  else parent->next = newPiece;
 /* headp->IDOfFrame                  = IDOfFrame;
  headp->size                       = size;
  headp->buffer                     = (unsigned char*)malloc(size);
  headp->pieceNumber                = pieceNumber;
  headp->numberOfPieces             = numberOfPieces; 
  headp->height                     = height;
  headp->width                      = width;
  memcpy(headp->buffer, buf, size);
  memcpy(headp->camID, camID, 4);
  memcpy(headp->macAddr, macAddr, 17);
  */
  //if ((*head) == NULL) *head = headp;
  pthread_mutex_unlock(&mutex);
}
int getFrame(Stack** head, unsigned char* frame, int* height, int* width)
{
  pthread_mutex_lock(&mutex);
  Stack* headp          = *head;
  int piece             = 0;
  int size              = 0;
  int shift             = 0;
  int IDOfFrame         = (*head)->IDOfFrame;
  int totalSize         = 0;
  printf("The size is %d\n", length(*head));
  while( headp != 0)
  {
    printf("cycle\n");
    memcpy(frame + shift, headp->buffer, headp->size);
    totalSize      += headp->size;
    shift          += headp->size;
    Stack* toDelete = headp;
    printf("pieceNumber = %d, numberOfPieces = %d\n", headp->pieceNumber, headp->numberOfPieces);
    if (headp->pieceNumber == headp->numberOfPieces)  //last piece
    {
      printf("the last\n");
      *height = headp->height;
      *width  = headp->width;
      *head   = headp->next;
      free(toDelete);
      pthread_mutex_unlock(&mutex);
      return totalSize;
    }
    if ((headp->next == 0) /*|| (headp->IDOfFrame != headp->next->IDOfFrame)*/) return -1;

    headp = headp->next;
    free(toDelete);
  }
  memset(frame, 0, totalSize);
  printf("There is no entire frame! The last piece is %d\n", headp->pieceNumber);
  pthread_mutex_unlock(&mutex);  
  return -1;
}
int mergeSort(Stack** head, int key1, int key2)
{
 /*
        I should realize here sorting function, because when I get pices of frames in a broken order, I should fix their order. There will not
        be mutex-locking, because this function will be usen only in  getFram() function. So, the mutext will be blocked by outer function.
        I should find somewhere for an answer for following question: Is there a sorting for lists that can replace only the one wring element.

 */
  return 0;
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
