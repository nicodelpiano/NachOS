// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
//#include "noff.h"
#include "mem_utils.h"

/*AGREGADOS PARA IMPLEMENTAR EXEC Y MULTIPROGRAMACION*/

#define FISPAGINA(i) (pageTable[(i)/PageSize].physicalPage * PageSize)
#define CODE(i) (&(machine->mainMemory[FISPAGINA(noffH.code.virtualAddr+i)+((noffH.code.virtualAddr + i) % PageSize)]))
#define DATA(i) (&(machine->mainMemory[FISPAGINA(noffH.initData.virtualAddr+i)+((noffH.initData.virtualAddr + i )% PageSize)]))

#define ISCODE(i) ((i)>=noffH.code.virtualAddr &&	(i)<noffH.code.virtualAddr + noffH.code.size)
#define ISDATA(i) ((i)>=noffH.initData.virtualAddr && (i)<noffH.initData.virtualAddr + noffH.initData.size)

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

//FISPAGINA(noffH.code.virtualAddr+i); //-----> inicio de la pagina donde va el byte i-esimo de codigo              
  			
//noffH.code.virtualAddr + i % PageSize; //---->offset dentro de la pagina


//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

//HACIENDO CAMBIOS

AddrSpace::AddrSpace(OpenFile *executable, int *size_addr)
{

    NoffHeader noffHead;
    unsigned int i, size;

    executable->ReadAt((char *)&noffHead, sizeof(noffHead), 0);
    if ((noffHead.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffHead.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffHead);
    ASSERT(noffHead.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffHead.code.size + noffHead.initData.size + noffHead.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);

    size = numPages * PageSize;

	stack_pointer = size;//la posicion del stack actual hasta donde se escribio	
	argv_reg = 0;
	argc_reg = 0;
	
	*size_addr = size;

    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('z', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
	
    //Nos fijamos si la cantidad de paginas que requiere el proceso
    //no excede la cantidad de paginas libres en la memoria fisica
    //ASSERT(numPages <= coremap->NumClear());

// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
	
//shadowTable (la borramos en... cuando matamos el addrspace asignado)
	shadowTable = new int[numPages];

    for (i = 0; i < numPages; i++) {
	//int bit = bitmap->Find();
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	pageTable[i].physicalPage = -1;
	//printf("POS %d\n",pageTable[i].virtualPage);
	pageTable[i].valid = false;
	pageTable[i].use = false;
	pageTable[i].dirty = false;
	pageTable[i].readOnly = false;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    // zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
	
	//shadowTable

	shadowTable[i] = 0;//seteamos todos los valores de la shadowtable como no cargados
	/*
		Cada indice de la shadowTable corresponde a una pagina virtual
		El valor de ese indice en la tabla shadow es si esta cargada en memoria fisica, swap, o no todavia, si es un zerofill, etc... (sacado del Road Map Through Nachos)
		0 = no cargado
		1 = esta en fisica
		2 = esta en swap
		3 = zerofill 

	*/
		
    	//	bzero(machine->mainMemory+bit*, PageSize);
		
    }
		
	noffH = noffHead;
	ejecutable = executable;
	#ifdef USE_TLB
	//Swap File, lo creamos y abrimos
	swapname = new char[8];
	sprintf(swapname,"%d.swap",spaceThread->getPid(currentThread));		
	fileSystem->Create(swapname, numPages*PageSize);
	swap = fileSystem->Open(swapname);
	#endif

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	DEBUG('z',"*******************************************Mato el ADDRSPACE\n");
	
		
	//borramos lo necesario creado cuando asignamos la memoria
	#ifdef USE_TLB	
	delete ejecutable;
	delete pageTable;
	fileSystem->Remove(swapname);//archivo swap
	delete shadowTable;
	delete swapname;	
	delete swap;	
	#else
	delete pageTable;
	#endif
	
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, stack_pointer);
	machine->WriteRegister(4,argc_reg);
	machine->WriteRegister(5,argv_reg);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
DEBUG('z',"*******************************************saveState\n");
#ifdef USE_TLB
	for(int i=0; i<TLBSize; i++)
		if(machine->tlb[i].valid)
			updatePageTable(machine->tlb[i].virtualPage,machine->tlb[i]);
#endif
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
	DEBUG('z',"*******************************************RestoreState\n");
	/* Tenemos que invalidar las entradas para que no 
	haya un problema en algun cambio de contexto */
	#ifdef USE_TLB
		for(int i=0;i<TLBSize;i++){
			machine->tlb[i].valid = false;
		}
	#else
    	machine->pageTable = pageTable;
    	machine->pageTableSize = numPages;
	#endif
}

#ifdef USE_TLB
/*
	IMPLEMENTACION DE TLB y VM
		|	|	|	|
		v	v	v	v
*/

//obtenemos la pagina victima
//desalojamos de acuerdo a una politica de paginacion
//usamos una lista global: pp_list
int AddrSpace::getVictim()
{
	DEBUG('k',"getVictim\n");
	int dir_fisica;//direccion fisica que vamos a obtener de la memoria fisica
				
	//politicas de desalojamiento!
	#ifdef FIFO
		DEBUG('k',"FIFO\n");
		dir_fisica = pp_list->Remove();//las paginas fueron cargadas cuando se necesitaban.
					//ahora hay que desalojar una (en el caso de fifo, la primera en entrar), para actualizarla
		pp_list->Append(dir_fisica);	
	#endif 
	
	//nos fijamos el bit R (use) de la pagina virtual que esta asociada
	//con la fisica que vamos a desalojar
	//si esta en false, la sacamos, sino la seteamos en false y "apuntamos la manecilla del reloj" a otra pag
	#ifdef RELOJ
		DEBUG('k',"RELOJ\n");
		while(1)
		{
			dir_fisica = pp_list->Remove();
			int vpn = coremap->vpn(dir_fisica);
			if(!coremap->owner(dir_fisica)->pageTable[vpn].use){
				pp_list->Append(dir_fisica);
				break;
			}
			else{
				pp_list->Append(dir_fisica);
				coremap->owner(dir_fisica)->pageTable[vpn].use = false;
			} 
		}
	#endif

	return dir_fisica;
}

/*
ClearPages
Borramos las paginas del coremap que ya no usamos
Esto lo hacemos al terminar el thread, como metodo para asegurarnos de que
un thread nuevo no se tope con paginas inexistentes en el coremap.
Ademas sacamos de pp_list, la pagina fisica (la ponemos como disponible)
*/
void AddrSpace::clearPages()
{
	for(int j=0;j<numPages;j++){
		int fp = pageTable[j].physicalPage;
		if(fp!=-1 && this == coremap->owner(fp))
		{	
			pp_list->RemoveOnce(fp);//borramos la pagina asociada
			coremap->ClearUp(fp,-1,NULL);						
		}
	}
}

/*
ValidEntry
Validamos una entrada (chequeamos que el indice de pagina virtual que nos dan este en los rangos
correctos!)
*/

bool AddrSpace::validEntry(int vp_index)
{
	//obtenemos la direccion de la pagina virtual
	DEBUG('k',"numPages:%d\n",numPages);
	if(vp_index>=numPages || vp_index<0)
		return false;

	return true;
	
}


/*
getEntry

Funcion que hace los chequeos del coremap y si hay que swapear o no.

Bien, nosotros usamos una shadowtable asociada a cada espacio de direcciones
(como sugiere el RoadMap) para ver donde esta ubicada cada entrada, es decir:
Supongamos la pagina virtual vp (indice), si todavia no fue cargada:
shadowTable[vp] = 0. (1 si esta cargada en fisica, 3 si es un zerofill, 2 si esta en un archivo swap)

Ahora, cuando recibimos un indice de pagina virtual, lo que hacemos es verificar el estado en la 
shadowtable para ver como proceder.

En caso de que la direccion virtual no este mapeada a la memoria fisica,
hacemos un Find en el coremap. Si esto dio Ok, perfecto, ahora bien si el bitmap del coremap da que no
hay un marco fisico libre,
lo que hacemos es desalojar un marco de pagina fisico con getVictim usando alguna
politica, y luego actualizamos el coremap, y swapeamos en caso de ser necesario.
*/

void AddrSpace::getEntry(int vp_index)
{
	DEBUG('k',"Vamos a cargar la entrada %d.\n",vp_index);
	
	int vaddr = (unsigned) vp_index*PageSize;//direccion virtual real

	DEBUG('k',"shadowTable[%d] = %d\n",vp_index,shadowTable[vp_index]);
	
	ASSERT(validEntry(vp_index));//validamos la entrada
	
	//vemos que hay en la shadowtable	
	switch(shadowTable[vp_index]){
			
			//si la pagina virtual vp_index no esta cargada
			case 0:{
				DEBUG('k',"La pagina %d no estaba asignada en memoria fisica.\n",vp_index);
							
				int fp = coremap->Find(vaddr,this);//buscamos una pagina
				
				//si no encontramos una pagina fisica libre
				if(fp==-1){ //esto se da cuando necesitamos desalojar y swapear
									
					fp = getVictim();//obtenemos una victima
					
					DEBUG('k',"Hay que swapear la pagina fisica %d, owner:%d , space:%d, vp:%d.\n",fp,coremap->owner(fp),currentThread->space,coremap->vpn(fp));

					//ahora la pagina virtual vieja no tiene asignada ninguna pagina fisica
					coremap->owner(fp)->saveToSwap(coremap->vpn(fp)/PageSize);//hacemos swap	
					//ponemos en no valida la pagina swapeada en la TLB			
					for (int i = 0; i < TLBSize; i++)
			            	if (machine->tlb[i].virtualPage == coremap->vpn(fp)/PageSize && this == coremap->owner(fp))
			                	machine->tlb[i].valid = false;					
					coremap->owner(fp)->pageTable[(coremap->vpn(fp)/PageSize)].physicalPage = -1;
					coremap->owner(fp)->pageTable[(coremap->vpn(fp)/PageSize)].valid = false;

					pageTable[vp_index].physicalPage = fp;
					pageTable[vp_index].use = true;
					coremap->Mark(fp,vaddr,this);//actualizamos el coremap
								
				}else{
					pageTable[vp_index].physicalPage = fp;
					pageTable[vp_index].use = true;					
					DEBUG('k',"Se encontro una pagina fisica libre: %d.\n",fp);
					pp_list->Append(fp);//ponemos la pagina al final
					
				}
				
				addToTLB(vaddr);

				cargarPagina(vp_index);	//cargamos la pagina						
				
				DEBUG('k',"Cargamos la pagina virtual %d, en la pagina fisica %d.\n",vp_index,pageTable[vp_index].physicalPage);
				ASSERT(pageTable[vp_index].physicalPage>=0);//cuando salga de aca tengo q tener asignada una pagina fisica!
				break;
			}
			//Si esta en memoria fisica, el pageFault fue porque la pagina no estaba en la TLB
			case 1:{ 
				DEBUG('k',"La pagina virtual %d ya se encuentra mapeada a fisica.\n",vp_index);
				ASSERT(pageTable[vp_index].physicalPage>=0);//deberia tener una pagina fisica asignada
				addToTLB(vaddr);				
								
				break;
			}
			//si esta en un archivo swap, hay que traerla de ese archivo y cargarla
			case 2:{
				DEBUG('k',"La pagina virtual %d esta en swap.\n",vp_index);
				int fp = coremap->Find(vaddr,this);//buscamos una pagina
				
				//si no encontramos una pagina fisica libre
				if(fp==-1)//hay que swapear una pagina
				{
					fp = getVictim();//obtenemos una fp victima
					
					DEBUG('k',"Hay que swapear la pagina fisica %d, owner:%d , vp:%d.\n",fp,coremap->owner(fp),coremap->vpn(fp));
					//ahora la pagina virtual vieja no tiene asignada ninguna pagina fisica
									
					coremap->owner(fp)->saveToSwap(coremap->vpn(fp)/PageSize);//hacemos swap
					
					//ponemos en no valida la pagina swapeada en la TLB
					for (int i = 0; i < TLBSize; i++)
			            	if (machine->tlb[i].virtualPage == coremap->vpn(fp)/PageSize && this == coremap->owner(fp))
			                	machine->tlb[i].valid = false;					
					coremap->owner(fp)->pageTable[(coremap->vpn(fp)/PageSize)].physicalPage = -1;			
					coremap->owner(fp)->pageTable[(coremap->vpn(fp)/PageSize)].valid = false;					
					
					pageTable[vp_index].physicalPage = fp;//la nueva le asignamos la pagina fisica victima					
					pageTable[vp_index].use = true;
					getFromSwap(vp_index);//traemos desde swap
					coremap->Mark(fp,vaddr,this);//actualizamos el coremap
				
				}else{				
					pageTable[vp_index].physicalPage = fp;
					pageTable[vp_index].use = true;					
					getFromSwap(vp_index);//traemos desde swap
					pp_list->Append(fp);//ponemos la pagina al final	
				}
				
				addToTLB(vaddr);//una vez asignada la fp a la dir virtual, la ponemos en la tlb
				
				ASSERT(pageTable[vp_index].physicalPage>=0);//deberia tener una pagina fisica asignada		
				
				break;
			}
			//si es un zerofill, hubo un pagefault porque no estaba en la tlb
			case 3:{
				DEBUG('k',"La pagina virtual %d es un zerofill.\n",vp_index);
				ASSERT(pageTable[vp_index].physicalPage>=0);
				addToTLB(vaddr);
				break;
			}
	}

	/*printTLB();
	printPT();
	coremap->Print();*/
	return;
}

/*
Cargamos una pagina que se quiera usar a la TLB (un pageFault por ejemplo).
Asigna un espacio a la direccion virtual recibida.
*/
void AddrSpace::addToTLB(int vaddr)
{
	DEBUG('k',"ADD TO TLB %d\n",vaddr/PageSize);

	int vp_index = vaddr/PageSize;//indice de la pagina virtual en la pagetable

	//recorremos la TLB
	for(int i=0;i<TLBSize;i++)
	{
		if(!machine->tlb[i].valid)//hay un slot vacio en la TLB
		{
			pageTable[vp_index].valid = true;//la validamos			
			machine->tlb[i] = pageTable[vp_index];//*te;
			return;
		}
	}
	
	//si esta llena la TLB, por ahora hacemos esto
	//tenemos un contador para ir viendo cual sacar
	//imitamos un estilo FIFO
	DEBUG('k',"Esta llena la TLB\n");
	DEBUG('k',"Counter %d, VP:%d\n",counter,vp_index);

	pageTable[vp_index].valid = true;//la validamos
	machine->tlb[counter] = pageTable[vp_index];//y ponemos en la tlb

	counter = (counter+1) % TLBSize;//cambiamos el contador global de la tlb
		
	return;
}


/*
Imprime la TLB actual
*/
void AddrSpace::printTLB()
{
	printf("========== Estado de la TLB ==========\n");
	for(int i=0;i<TLBSize;i++)
	{	
		printf("Entrada: tlb[%d] valid:%d virtPage:%d fisPage:%d\n",i,machine->tlb[i].valid?1:0,machine->tlb[i].virtualPage,machine->tlb[i].physicalPage);
	}
	printf("======================================\n");
}

/*
Imprime la pagetable del currentThread
*/
void AddrSpace::printPT()
{
	printf("========== Estado de la PageTable del SpaceThread %d ==========\n",currentThread->space);
	for(int i=0;i<numPages;i++)
	{	
		printf("Entrada: PageTable[%d] valid:%d virtPage:%d fisPage:%d shadowTable[%d]:%d\n",i,pageTable[i].valid?1:0,pageTable[i].virtualPage,pageTable[i].physicalPage,pageTable[i].virtualPage,shadowTable[pageTable[i].virtualPage]);
	}
	printf("===============================================================\n");
}

/*
cargarPagina
carga la pagina (lo que escribio Fede en clase!)
modifica el shadowtable de la vp recibida!
*/
void AddrSpace::cargarPagina(int vp)
{
	DEBUG('k',"Cargamos la pagina %d\n",vp);
	int vaddr = vp * PageSize;
	OpenFile *exe = ejecutable;
	
	DEBUG('k',"noffH.code.size:%d\n",noffH.code.size);
	DEBUG('k',"noffH.initData.size:%d\n",noffH.initData.size);

	shadowTable[vp] = 1;

	for(int i = vaddr;i<vaddr+PageSize;i++){
		char byte;
		if (ISCODE(i) && noffH.code.size>0) {

			    DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
		                    i, noffH.code.size);
			  
				exe->ReadAt(&byte,1,noffH.code.inFileAddr + (i - noffH.code.virtualAddr));
				WRITEMEM(i,1,byte);
				
				
		}else if(ISDATA(i) && noffH.initData.size>0){

				DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
							noffH.initData.virtualAddr, noffH.initData.size);
			
				exe->ReadAt(&byte,1,noffH.initData.inFileAddr + (i - noffH.initData.virtualAddr));
				WRITEMEM(i,1,byte);
		}else{
				shadowTable[vp] = 3;
				WRITEMEM(i,1,0);
		}
	}
	
}
#endif

/*
Funciones para cargar y sacar una pagina del swap

Vease que modifican el shadowtable!	
*/

void AddrSpace::saveToSwap(int vpn){
 
	char page[PageSize];
	DEBUG('k',"trying to swap\n");
	DEBUG('k',"S:%d\n",pageTable[vpn].physicalPage);
	for(int i = 0; i<PageSize;i++){
		page[i] = machine->mainMemory[(pageTable[vpn].physicalPage * PageSize) + i];
		DEBUG('k',"%d",page[i]);
	}
	
	DEBUG('k',"\nswapped\n");
	swap->WriteAt(page,PageSize,vpn*PageSize);
	shadowTable[vpn]=2;//boom, swapeada!
}

void AddrSpace::getFromSwap(int vpn){

	//buscar una pagina y completar la entrada de la page table
	char page[PageSize];
	
	swap->ReadAt(page,PageSize,vpn*PageSize);
	
	for(int i=0;i<PageSize;i++)
		machine->mainMemory[(pageTable[vpn].physicalPage * PageSize)+i] = page[i];

	shadowTable[vpn]=1;//boom, a memoria fisica otra vez!

}
