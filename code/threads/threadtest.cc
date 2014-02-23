// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "puerto.h"
#include "unistd.h"
#include <stdlib.h>
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

Lock* lock1 = new Lock("lock1");//agregamos esto para probar el caso de locks
Lock* lock2 = new Lock("lock2");//agregamos esto para probar el caso de locks
//Lock* lock2 = new Lock("lock2");

/*class Buffer{
 public:
	Buffer();			// initialize the list
        ~Buffer();
	int sacar();
	void poner(int j);
	bool estalleno();
	bool estavacio();
 private:
	int buff[10];
	int pos;		
}buffer;

Buffer::Buffer(){

	pos = 0;

}

Buffer::~Buffer(){}


int Buffer::sacar(){

	if(!estavacio()){
		pos--;
		//buff[pos+1] = NULL;
		return buff[pos+1];
	}

}

void Buffer::poner(int j){

	if(!estalleno()){
		buff[pos+1] = j;
		pos++;	
	}	

}

bool Buffer::estalleno()
{
	return (pos == 9);
}

bool Buffer::estavacio()
{
	return (pos == 0);
}

Buffer *buff1 = new Buffer();
*/
Puerto *puerto = new Puerto("puerto");

int bufferr[30];

Condition* lleno = new Condition ("lleno",lock1);
Condition* vacio = new Condition ("vacio",lock1);

void productor(void* name){
	while(1){
		/*lock1->Acquire();
		while(buff1->estalleno()){	 		 
			//printf("Bufferlleno\n");
			lleno->Wait();		
		}*/
		puerto->send(1);
		printf("send 1 %s\n",currentThread->getName());		
		/*vacio->Signal();
		lock1->Release();
		currentThread->Yield();*/
	}
}

void consumidor(void* name){
int j;
	while(1){
		/*lock1->Acquire();
		while (buff1->estavacio()){
			//printf("BufferVacio\n");
			vacio->Wait();    //Se libera el lock lock y se detiene el hilo.
		}*/
	puerto->receive(&j);
	printf("recibe %d %s\n",j,currentThread->getName());
	/*lleno->Signal();	
	lock1->Release();
 	currentThread->Yield();*/
	}
}

void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
	
    //lock->Acquire();//agregamos esto para probar el caso de locks

    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
	

    //lock->Release();//agregamos esto para probar el caso de locks

    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------
void fun2(void* name)
{
  printf("LA2\n");
}

void fun3(void* name)
{
  printf("LA3\n");
}

void fun(void* name)
{
//sleep(1);
printf("hola\n");
  char* threadname = new char[100];
  Thread* newThread2 = new Thread (threadname,true,0);
  newThread2->Fork (fun2,(void*)threadname); newThread2->Join();
  //while(1){}
printf("LA\n");
}

Lock* lock3 = new Lock("lock3");
Semaphore* sem = new Semaphore("sem",0);
Semaphore* sem2 = new Semaphore("sem2",0);

Semaphore* s = new Semaphore("s",0);
Semaphore* s2 = new Semaphore("s2",0);


//Inversion de prioridades

/*void Alta(void* str) {
    
    //Esperamos un mensaje para dormir el proceso
    s->P();
    
    lock3->Acquire();
    DEBUG('t', "Soy el proceso de prioridad ALTA\n");
    lock3->Release();
}

void Media(void* str) {
    s2->P();
    //while (1);
    int i, j;
	printf("MEDIA MEDIA\n");
    for (i = 0; i < 100000; i++)
      for (j = 0; j < 10000; j++);
}

void Baja(void* str) {
    lock3->Acquire();
    
    //Despues de tomar el lock, enviamos un mensaje a PAlta para despertarlo y hacemos un Yield para que 
    //el scheduler lo empiece a ejecutar, teniendo PBaja el lock tomado
    s->V();
    s2->V();
    //currentThread->Yield();
    
    DEBUG('t', "Soy el proceso de prioridad BAJA\n");
    lock3->Release();
}
*/void Alta(void* name)
{
	sem->P();
	lock3->Acquire();
	if(1)printf("Soy alta\n");
	lock3->Release();
}

void Media(void* name)
{
	sem2->P();
	if(1)printf("Soy media\n");
}

void Baja(void* name)
{
	
	lock3->Acquire();
	sem->V();
	sem2->V();
	currentThread->Yield();
	if(1)printf("Soy baja\n");
	lock3->Release();
	
}

void
ThreadTest()
{
   char* threadname = new char[100];
    DEBUG('t', "Entering SimpleTest");
	threadname = "BAJA";
    Thread* newThread = new Thread (threadname,false,0);
	threadname = "MEDIA";
	Thread* newThread3 = new Thread (threadname,false,1);
	threadname = "ALTA";
	Thread* newThread2 = new Thread (threadname,false,2);
	    
	newThread->Fork(Baja,(void*)threadname);
	newThread3->Fork(Media,(void*)threadname);
	newThread2->Fork(Alta,(void*)threadname);
    







   /*printf("lalala");
    newThread->Join();
    char* threadname = new char[100];
  Thread* newThread2 = new Thread (threadname,true);
  newThread2->Fork (fun2,(void*)threadname);  
  newThread2->Join();
   for ( int k=1; k<=1; k++) {
      printf("lll\n");
   char* threadname = new char[100];
      sprintf(threadname, "Hilo %d", k);
      Thread* newThread2 = new Thread (threadname,true);
       newThread->Fork (SimpleThread, (void*)threadname);
	  if((k%2)==0) 
		newThread->Fork (fun2, (void*)threadname);
	  else
	    
			newThread2->Fork (fun2,(void*)threadname);   newThread2->Join();
	}
    
   SimpleThread( (void*)"Hilo 0");
*/
  
}

