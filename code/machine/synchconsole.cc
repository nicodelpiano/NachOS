#include "copyright.h"
//#include "system.h"
#include "synchconsole.h"
#include "synch.h"

Semaphore *sreadAvail;
Semaphore *swriteDone;

static void ReadAvail(void* arg) { sreadAvail->V(); }
static void WriteDone(void* arg) { swriteDone->V(); }

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{

        sreadAvail = new Semaphore("readAvail",0);
        swriteDone = new Semaphore("writeDone",0);
        lock1 = new Lock("Synch lock1");
        lock2 = new Lock("Synch lock2");
        console = new Console(readFile,writeFile,ReadAvail,WriteDone,this);

}

SynchConsole::~SynchConsole()
{
        //delete semaphore;
        delete lock1;
        delete lock2;
        delete console;
}

void SynchConsole::PutChar(char ch)
{
        lock1->Acquire();
        console->PutChar(ch);
	swriteDone->P();
        lock1->Release();

}

char SynchConsole::GetChar()
{
        char c;
	lock2->Acquire();
        sreadAvail->P();
        c = console->GetChar();
        lock2->Release();
	return c;
}

