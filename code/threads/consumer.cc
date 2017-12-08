#include"consumer.h"
#include<stdio.h>
#include "synch.h"
#include"thread.h"
extern Thread *currentThread;
Consumer::Consumer(List<int> *list, Lock *lock, Condition *cond, Condition *cond1, int conId){
    sharedList = list;
    consumerLock = lock;
    consumerCond = cond;
    producerCond = cond1;
    element = 0;
    id = conId;
}

Consumer::~Consumer(){
    delete sharedList;
}

void Consumer::consume(void* value){
    int s = (rand() % 2) + 1;
    int e = (rand() % 2) + 1;
    while(true){
      consumerLock->Acquire();
      while(s-- != -1);
      while (sharedList->IsEmpty()) {
          printf("List is empty.Consumer %d is now waiting\n", id);
          consumerCond->Wait(consumerLock);
      }
      element = sharedList->Remove();
      printf("Consumer %d consumed %d list_size %d\n",id,element,sharedList->getSize());
      producerCond->Broadcast(consumerLock);
      consumerLock->Release();
      currentThread->Yield();
      while (e-- != -1);
    }
}
