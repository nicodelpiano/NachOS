#include "coremap.h"

/*
Preferimos iniciar la estructura del coremap asi:
Si no tiene un addrspace asignado, es NULL, y en caso de no tener 
asignada una memoria virtual, es -1.
*/
CoreMap::CoreMap(int nfpages){

	counter = 0;
	npages = nfpages;
	bitmap = new BitMap(nfpages);
	coremap = new fpage_info[nfpages];

	for(int i=0;i<nfpages;i++)
	{
		coremap[i].vpage = -1;
		coremap[i].aspace = NULL;
	}
}
CoreMap::~CoreMap(){

	delete coremap;	

}

int CoreMap::NumClear()
{
	return bitmap->NumClear();
}

void CoreMap::Mark(int fpage,int virp,AddrSpace *addr)
{
	if(bitmap->Test(fpage))
		bitmap->Mark(fpage);
	coremap[fpage].vpage = virp;
	coremap[fpage].aspace = addr;
}
//hace un mark, sin alterar el bitmap y borrando la entrada 
//en el bitmap
void CoreMap::ClearUp(int fpage,int virp,AddrSpace *addr)
{
	bitmap->Clear(fpage);
	coremap[fpage].vpage = virp;
	coremap[fpage].aspace = addr;
}

int CoreMap::Find(int virtpage,AddrSpace *addr)
{		
	int bit = bitmap->Find();	
	if(bit<0)
	{
		return -1;
	}
	Mark(bit,virtpage,addr);
	return bit;
	
}

AddrSpace* CoreMap::owner(int fpage)
{
	return(coremap[fpage].aspace);
}

int CoreMap::vpn(int fpage)
{
	return(coremap[fpage].vpage);
}

void CoreMap::Print()
{
	printf("\n==========Estado del coremap:==========\n");
	for(int i=0;i<npages;i++)
		printf("FisAddr:%d VirtAddr:%d Thread:%d\n",i,coremap[i].vpage,coremap[i].aspace);
	printf("=======================================\n");
}
