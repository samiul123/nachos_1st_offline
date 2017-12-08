#include<stdio.h>
#include"producer.h"
#include "synch.h"
#include<cstdlib>
#include"thread.h"
extern Thread *currentThread;
Producer::Producer(List<int> *list, Lock *lock, Condition *cond, Condition *cond1, int proId){
    sharedList = list;
    element = 0;
    MAX_SIZE = 5;
    producerLock = lock;
    producerCond = cond;
    consumerCond = cond1;
    id = proId;
}

Producer::~Producer(){
    delete sharedList;
}

void Producer::produce(void* value){
    int s = (rand() % 2) + 1;
    int e = (rand() % 2) + 1;
    while(true){
      producerLock->Acquire();
      while(s-- != -1);
      //int value = *((int*)element);
      //value++;
      int r = rand() % 200;
      element += r;
      while (sharedList->getSize() == MAX_SIZE) {
          printf("List is full.Producer %d is now waiting\n",id);
          producerCond->Wait(producerLock);
      }
      sharedList->Append(element);
      printf("Producer %d produced %d list_size %d\n",id,element,sharedList->getSize());
      consumerCond->Broadcast(producerLock);
      producerLock->Release();
      currentThread->Yield();
      while (e-- != -1);
    }

}
