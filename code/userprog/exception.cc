// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
//#include "noff.h"
#include "mem_utils.cc"
#include <stdlib.h>
//#include "spacethread.h"

#define BUFF_LEN 128

#define WRITEMEM(d,s,v) if(!machine->WriteMem(d,s,v)) ASSERT(machine->WriteMem(d,s,v))
#define READMEM(d,s,v) if(!machine->ReadMem(d,s,v)) ASSERT(machine->ReadMem(d,s,v))
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

/*estructura para guardar los datos que necesitamos
para hacer el exec con varios argumentos*/
struct execDat{
	OpenFile *e_exec; //el ejecutable
	int e_argc; //cant args
	char **e_argv; //los argumentos
};

void startProc(void *eDat)
{	
	//copiamos la info recibida en eDat
	execDat *ed = (execDat*) eDat;
	int argc = ed->e_argc;
	char **argv = ed->e_argv;
	OpenFile *exec = ed->e_exec;
	int size;//size que le damos a addrspace para que nos guarde numPages*pageSize ya que no tenemos acceso a los mismos

	AddrSpace *addr = new AddrSpace(exec,&size);
		
	//delete exec;
	currentThread->space = addr;

	//vamos a escribir en el stack
	//seteamos el stack pointer(addrp) al fondo
	int addrp = size;	
	addrp &=(-4);
	int addr_array[argc];//aca guardamos los punteros del comienzo de cada argumento

	for(int j=0;j<argc;j++) {
		addrp = addrp - strlen(argv[argc-j-1]) -1;
		addrp &= (-4);//alineo las direcciones por las dudas!
		addr_array[argc-j-1] = addrp;
		currentThread->space->RestoreState();
		writeStrToUsr(argv[argc-j-1], addrp);//argc-j-1
		//machine->WriteMem(argvaddr+4*argc, 4, addr);
	}
	
	for(int j=0;j<argc;j++){
		addrp = addrp - 4;
		addrp &= (-4);//alineo las direcciones
		WRITEMEM(addrp,4,addr_array[j]);
		DEBUG('j',"addr: %d\n",addrp);			
	}
	int argv_addr = addrp;
	addrp = addrp - 16; //16 magicos
	DEBUG('j',"addr: %d\n",addrp);
	// los punteros de los argumentos
	DEBUG('j',"Argv addr: %d\n",argv_addr);	
	//argv_addr &= ~0x3; 	
	
	//seteamos los valores para los registros
	currentThread->space->stack_pointer = addrp;//la posicion del stack actual hasta donde se escribio
	currentThread->space->argv_reg = argv_addr;
	currentThread->space->argc_reg = argc;
	
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	

	delete eDat;
	machine->Run();
	
}

void incrementarPC(){

        int i;
        
        i = machine->ReadRegister(34);//leemos el valor del actual
        machine->WriteRegister(36,i);//guardamos este valor en el registro previo
        i = machine->ReadRegister(35);//leemos el valor del siguiente
        machine->WriteRegister(34,i);//guardamos en el actual
        machine->WriteRegister(35,i+4);//en el siguiente ponemos el otro
        
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException)) {
		switch(type){
			case SC_Halt:{
					DEBUG('a', "Shutdown, initiated by user program.\n");
					interrupt->Halt();
					break;
			}
			case SC_Create:{
					int name_addr = machine->ReadRegister(4); //argumento 1
					char name[BUFF_LEN];
					readStrFromUsr(name_addr,name);
					DEBUG('j',"Creé %s\n",name);
					fileSystem->Create(name, 1024);
					machine->WriteRegister(2,0);
					break;
			}
			case SC_Open:{
					int name_addr = machine->ReadRegister(4); //argumento 1
					char *name = new char[BUFF_LEN];
					DEBUG('j',"Abri %d\n",name_addr);
					readStrFromUsr(name_addr,name);
					DEBUG('j',"Abri %s\n",name);
					OpenFile *f = fileSystem->Open(name);
					if(f == NULL){
						machine->WriteRegister(2,-1);
						break;				
					}
					int fd = currentThread -> addFile(f);
					delete name;
					machine->WriteRegister(2,fd);		  
					break;
			}
			case SC_Write:{
			
					char escribe[BUFF_LEN];
					int usr_fd = machine->ReadRegister(6);
					int usr_addr = machine->ReadRegister(4); //argumento 1
					int size = machine->ReadRegister(5);
		    		
					int i;
					
					readBuffFromUsr(usr_addr,escribe,size);			
			
					if(usr_fd == ConsoleInput)
					{				
						break;//no podemos escribir en la entrada estandar
					}
					if(usr_fd == ConsoleOutput)
					{
						for(i=0;i<size;i++){
							synchConsole->PutChar(escribe[i]);
							//DEBUG('j',"Escribi");
						}
					}else{
						OpenFile *f = currentThread->getFile(usr_fd);
						f->Write(escribe,size);
						DEBUG('j',"Escribi %s\n",escribe);
					}					
					break;
			}
			case SC_Read:{

					char leido[BUFF_LEN];

					int addr = machine->ReadRegister(4);
					int usr_fd = machine->ReadRegister(6);
					int size = machine->ReadRegister(5);
					int i;
					
					if(usr_fd == ConsoleOutput)
					{
						break;//no podemos leer de la salida estandar
					}
					if(usr_fd == ConsoleInput)
					{
						for(i=0;i<size;i++){
							leido[i] = synchConsole->GetChar();
							DEBUG('j',"Lei %c",leido[i]);
						}
						writeBuffToUsr(leido,addr,size);
						machine->WriteRegister(2,i);
					}else{
						OpenFile *f = currentThread->getFile(usr_fd);
						machine->WriteRegister(2,f->Read(leido,size));	
						writeBuffToUsr(leido,addr,size);					
						DEBUG('j',"Lei %s\n",leido);	
					}
					break;
			}
			case SC_Close:{

					int id = machine->ReadRegister(4); //argumento 1
		
					DEBUG('j',"Cierro %d\n",id);
		
					delete currentThread->RemoveFid(id);
					//fileSystem->Close(id);
					machine->WriteRegister(2,0);

					break;
			}
			case SC_Exit:{

					int status = machine->ReadRegister(4); //argumento 1
					
					spaceThread->setRetValue(currentThread,status);
					
					//delete currentThread->space;
					//currentThread->space = NULL;
					DEBUG('z',"EXIT : %d\n",status);					
					currentThread->Finish();
					break;			
			}
			case SC_Exec:{
					/*Estamos haciendo cambios para exec*/
					char * name = new char[BUFF_LEN];
					int argc, dir;

					execDat *eDat = new execDat;

					readStrFromUsr(machine->ReadRegister(4),name);
					DEBUG('j',"El proceso %s llama a Exec\n",name);
					
					argc = machine->ReadRegister(5);					
					dir = machine->ReadRegister(6);
			
					char **arreglo = new char*[argc];
					for(int j=0;j<argc;j++)
						arreglo[j] = new char[BUFF_LEN];
					int usrAddr;					

					OpenFile *exec = fileSystem->Open(name);

					if(exec==NULL)
					{
						DEBUG('j',"No se pudo abrir el archivo %s.\n",name);
						delete exec;
						machine->WriteRegister(2,-1);						
						break;
					}					

					for(int i=0;i<argc;i++)
					{
						int direc = 0;						
						READMEM(dir+i*4,4,&direc);						
						readStrFromUsr(direc,(char*)(arreglo[i]));
						DEBUG('j',"ARREGLO: %s",arreglo[i]);
					}
					
					/*AddrSpace *addr = new AddrSpace(exec);
					delete exec;
					*/
					Thread *t = new Thread(name,true,currentThread->getPriority());
					
					//t->space = addr;

					SpaceId pid = spaceThread->addThread(t);
					
					if(pid==-1)
					{
						DEBUG('j',"Error, se excede el maximo de threads permitidos\n");
						machine->WriteRegister(2,-1);
						break;
					}

					eDat->e_exec = exec;
					eDat->e_argc = argc;
					eDat->e_argv = arreglo;
					
					t->Fork(startProc,(void*) eDat);
						
					machine->WriteRegister(2,pid);

					break;
				}
			case SC_Join:{
					DEBUG('j',"Se llama a Join\n");
					int pid = machine->ReadRegister(4);

					if(pid<0){
						machine->WriteRegister(2,-1);
						break;
					}
					
					Thread *t = spaceThread->getThread(pid);

					if(t!=NULL)
					{				
						t->Join();
						
						int ret = spaceThread->getRetValue(pid);
												
						machine->WriteRegister(2,ret);
						
					}else{
						
						machine->WriteRegister(2,-1);					
					}
					break;
				}	
			
		}
		
		incrementarPC();

	#ifdef USE_TLB
	}else if(which == PageFaultException){
		
		DEBUG('k',"PageFaultException\n");
		int addr = machine->ReadRegister(BadVAddrReg);
		
		TranslationEntry *te = new TranslationEntry;
		
		currentThread->space->getEntry(addr/PageSize);
		
	}else if(which == ReadOnlyException){
		
	#endif
	}else{
		printf("Unexpected user mode exception %d %d\n", which, type);
					ASSERT(false);
	}
	  
    
}
