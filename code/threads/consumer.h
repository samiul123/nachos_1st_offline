#include"thread.h"
#include "list.h"
#include "synch.h"


class Consumer{
private:
    List<int> *sharedList;
    int element;
    Lock *consumerLock;
    Condition *consumerCond;
    Condition *producerCond;
    int id;
public:
    Consumer(List<int> *sharedList, Lock *consumerLock, Condition *consumerCond, Condition *producerCond, int conId);
    ~Consumer();
    void consume(void* element);
};
