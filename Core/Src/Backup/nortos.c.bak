/*
 * nortos.c
 *
 *  Created on: Sep 3, 2022
 *      Author: pinelab
 */


#include "nortos.h"

#define Q_SIZE_FAST 16

volatile int F1_last; // number of last element of fast-speed queue
int F1_first; // number of first element of fast-speed queue

void SimpleF1(){;};

void (*F1_Queue[Q_SIZE_FAST])();

 void F1_QueueIni(void){ // initialization of Queue
  F1_last = 0;
  F1_first = 0;
}

 int F1_push(void (*pointerQ)(void) ){ // push element from the queue
  if ((F1_last+1)%Q_SIZE_FAST == F1_first)return 1;
  F1_Queue[F1_last++] = pointerQ;
  F1_last%=Q_SIZE_FAST;
  return 0;
}

 void (*F1_pull(void))(void){ // pull element from the queue
  void (*pullVar)(void);
  if (F1_last == F1_first)return SimpleF1;
  pullVar = F1_Queue[F1_first++];
  F1_first%=Q_SIZE_FAST;
  return pullVar;
}

