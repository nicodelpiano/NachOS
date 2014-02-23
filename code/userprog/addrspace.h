// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"
#include "translate.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable,int *size);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 

	//PARAMETROS DEL STACK
	int stack_pointer;//la posicion del stack actual hasta donde se escribio
	int argv_reg;
	int argc_reg;

	//TLB y VM
	bool validEntry(int vp_index);
	void addToTLB(int vaddr);
	
	void printTLB();
	
	OpenFile *ejecutable; //programa actual que reserva la memoria
	void cargarPagina(int vp);
	NoffHeader noffH;

	//swap
	void saveToSwap(int vpn);
	void getFromSwap(int vpn);

	int getVictim();//sacamos una direccion de la memoria fisica
	void updatePageTable(int vaddr, TranslationEntry te){pageTable[vaddr]= te;}
	void getEntry(int vp_index);
	void printPT();

	void clearPages();	

  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

	OpenFile *swap;
	char *swapname;

	int *shadowTable;
	/*
		Cada indice de la shadowTable corresponde a una pagina virtual
		El valor de ese indice en la tabla shadow es si esta cargada en memoria fisica, swap, o no todavia, si es un zerofill, etc... (sacado del Road Map Through Nachos)
		0 = no cargado
		1 = esta en fisica
		2 = esta en swap
		3 = zerofill 

	*/

};

#endif // ADDRSPACE_H
