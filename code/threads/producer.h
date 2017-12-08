#include"thread.h"
#include "list.h"
#include "synch.h"

class Producer{
private:
    List<int> *sharedList;
    int MAX_SIZE;
    int element;
    Lock *producerLock;
    Condition *producerCond;
    Condition *consumerCond;
    int id;
public:
    Producer(List<int> *sharedList, Lock *producerLock, Condition *producerCond, Condition *consumerCond, int proId);
    ~Producer();
    void produce(void* element);
};
