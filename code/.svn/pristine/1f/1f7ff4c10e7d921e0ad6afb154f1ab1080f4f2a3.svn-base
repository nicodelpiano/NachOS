// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

//ponemos la maxima prioridad n, pero va de 0 a n.
//en este caso las prioridades van de 0 a 2
#define MAX_PRIOR 2 

//sabemos que en realidad no es la definicion exacta pero la usamos asi
//las listas son una buena implementacion de colas
typedef List<Thread*> Cola;

// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.

class Scheduler {
  public:
    Scheduler();			// Initialize list of ready threads 
    ~Scheduler();			// De-allocate ready list

    void ReadyToRun(Thread* thread);	// Thread can be dispatched.
    Thread* FindNextToRun();		// Dequeue first thread on the ready 
					// list, if any, and return thread.
    void Run(Thread* nextThread);	// Cause nextThread to start running
    void Print();			// Print contents of ready list
    
    void SwitchPriority(Thread* t, int p);//agregado para cambiar la prioridad de un proceso

  private:
    Cola *readyList[MAX_PRIOR+1]; //multicola. La prioridad de un hilo esta entre 0 y MAX_PRIOR asi que el arreglo debe tener MAX_PRIOR+1 lugares
	//List<Thread*> *readyList;// queue of threads that are ready to run,
					// but not running
};

#endif // SCHEDULER_H
