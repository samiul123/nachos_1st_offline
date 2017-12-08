// main.cc
//	Bootstrap code to initialize the operating system kernel.
//
//	Allows direct calls into internal operating system functions,
//	to simplify debugging and testing.  In practice, the
//	bootstrap code would just initialize data structures,
//	and start a user program to print the login prompt.
//
// 	Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"
#include "synch.h"
#include "producer.h"
#include "consumer.h"
#include "thread.h"

// External functions used by this file

void ThreadTest();
void Copy(const char *unixFile, const char *nachosFile);
void Print(const char *file);
void PerformanceTest(void);
void StartProcess(const char *file);
void ConsoleTest(const char *in, const char *out);
void MailTest(int networkID);

//----------------------------------------------------------------------
// main
// 	Bootstrap the operating system kernel.
//
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int arr_size = 5;
List<int> sharedList;
Lock globalLock((char*)"Global_lock");
Condition fullCond((char*)"Producer_full_condition");
Condition emptyCond((char*)"Consumer_empty_condition");
int value = 89;

Producer *producer1 = new Producer(&sharedList, &globalLock, &fullCond, &emptyCond, 1);
Consumer *consumer1 = new Consumer(&sharedList, &globalLock, &emptyCond, &fullCond, 1);
Thread producerThread1((char*)"Producer_thread_1");
Thread consumerThread1((char*)"Consumer_thread_1");

Producer *producer2 = new Producer(&sharedList, &globalLock, &fullCond, &emptyCond, 2);
Consumer *consumer2 = new Consumer(&sharedList, &globalLock, &emptyCond, &fullCond, 2);
Thread producerThread2((char*)"Producer_thread_2");
Thread consumerThread2((char*)"Consumer_thread_2");

Producer *producer3 = new Producer(&sharedList, &globalLock, &fullCond, &emptyCond, 3);
Consumer *consumer3 = new Consumer(&sharedList, &globalLock, &emptyCond, &fullCond, 3);
Thread producerThread3((char*)"Producer_thread_3");
Thread consumerThread3((char*)"Consumer_thread_3");

void producerFuncWrapper1(void* arg){
  producer1->produce(arg);
}

void producerFuncWrapper2(void* arg){
  producer2->produce(arg);
}

void producerFuncWrapper3(void* arg){
  producer3->produce(arg);
}

void consumerFuncWrapper1(void* arg){
  consumer1->consume(arg);
}

void consumerFuncWrapper2(void* arg){
  consumer2->consume(arg);
}

void consumerFuncWrapper3(void* arg){
  consumer3->consume(arg);
}

// struct ProducerInfo{
//     Producer *producer;
//     void *arg;
// };
// struct ConsumerInfo{
//     Consumer *consumer;
//     void *arg;
// };

int main(int argc, char **argv)
{
    int argCount;			// the number of arguments
					// for a particular command

    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);

#ifdef THREADS
    ThreadTest();
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	       argCount = 1;
        if (!strcmp(*argv, "-z"))               // print copyright
            printf ("%s",copyright);
#ifdef USER_PROGRAM
        if (!strcmp(*argv, "-x")) {        	// run a user program
	          ASSERT(argc > 1);
            StartProcess(*(argv + 1));
            argCount = 2;
        }
        else if (!strcmp(*argv, "-c")) {      // test the console
	          if (argc == 1)
	             ConsoleTest(NULL, NULL);
	          else {
		           ASSERT(argc > 2);
	             ConsoleTest(*(argv + 1), *(argv + 2));
	             argCount = 3;
	          }
	    interrupt->Halt();		// once we start the console, then
					// Nachos will loop forever waiting
					// for console input
	}
#endif // USER_PROGRAM
#ifdef FILESYS
	if (!strcmp(*argv, "-cp")) { 		// copy from UNIX to Nachos
	    ASSERT(argc > 2);
	    Copy(*(argv + 1), *(argv + 2));
	    argCount = 3;
	} else if (!strcmp(*argv, "-p")) {	// print a Nachos file
	    ASSERT(argc > 1);
	    Print(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-r")) {	// remove Nachos file
	    ASSERT(argc > 1);
	    fileSystem->Remove(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-l")) {	// list Nachos directory
            fileSystem->List();
	} else if (!strcmp(*argv, "-D")) {	// print entire filesystem
            fileSystem->Print();
	} else if (!strcmp(*argv, "-t")) {	// performance test
            PerformanceTest();
	}
#endif // FILESYS
#ifdef NETWORK
        if (!strcmp(*argv, "-o")) {
	          ASSERT(argc > 1);
            Delay(2); 				// delay for 2 seconds
						// to give the user time to
						// start up another nachos
            MailTest(atoi(*(argv + 1)));
            argCount = 2;
        }
#endif // NETWORK
    }
    int *val;
    val = &value;
    int div1 = 0;
    int div2 = 0;
    int div3 = 0;
    int div4 = 0;
    int div5 = 0;
    int div6 = 0;
    while (true) {
        int r = (rand() % 10) + 1;
        if(r == 1 && div1 == 0){
          producerThread1.Fork(producerFuncWrapper1, (void*)val);
          div1 = 1;
        }
        else if(r == 2 && div2 == 0){
          consumerThread1.Fork(consumerFuncWrapper1, (void*)val);
          div2 = 1;
        }
        else if(r == 3 && div3 == 0){
            producerThread2.Fork(producerFuncWrapper2, (void*)val);
            div3 = 1;
        }
        else if(r == 4 && div4 == 0){
            consumerThread2.Fork(consumerFuncWrapper2, (void*)val);
            div4 = 1;
        }
        else if(r == 5 && div5 == 0){
            producerThread3.Fork(producerFuncWrapper3, (void*)val);
            div5 = 1;
        }
        else if(r == 6 && div6 == 0){
            consumerThread3.Fork(consumerFuncWrapper3, (void*)val);
            div6 = 1;
        }
        else if(div1 == 1 && div2 == 1 && div3 == 1 && div4 == 1 && div5 == 1 && div6 == 1) break;
    }


    // producerThread1.Fork(producerFuncWrapper1, (void*)val);
    // consumerThread1.Fork(consumerFuncWrapper1, (void*)val);
    // consumerThread2.Fork(consumerFuncWrapper2, (void*)val);
    // producerThread2.Fork(producerFuncWrapper2, (void*)val);
    // producerThread3.Fork(producerFuncWrapper3, (void*)val);
    // consumerThread3.Fork(consumerFuncWrapper3, (void*)val);

    currentThread->Finish();	// NOTE: if the procedure "main"
				// returns, then the program "nachos"
				// will exit (as any other normal program
				// would).  But there may be other
				// threads on the ready list.  We switch
				// to those threads by saying that the
				// "main" thread is finished, preventing
				// it from returning.
    return(0);			// Not reached...
}
