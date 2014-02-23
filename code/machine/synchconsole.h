#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"

//#ifndef Included_NameModel_H
//#define Included_NameModel_H

extern Semaphore *sreadAvail;
extern Semaphore *swriteDone;

class SynchConsole {
  public:
    SynchConsole(const char *readFile, const char *writeFile);

    ~SynchConsole();		
    
    void PutChar(char ch);

    char GetChar();


  private:
    //Semaphore *semaphore; 		
    Lock* lock1;
    Lock* lock2;
    Console* console;
    
};

//#endif
