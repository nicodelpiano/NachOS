#include "syscall.h"
#include "addrspace.h"
#include "thread.h"
#include "bitmap.h"

/*

Extraido del Road Map:

You will need to maintain a data structure called a core map. The core map is a table containing an entry for every physical page frame in the system. For each page frame, a 
core map entry keeps track of:

Is the page frame allocated or free?
Which address space is using this page?
which virtual page number within space?

*/

typedef struct _fpage_info{

	//bool allocated;//Is the page frame allocated or free? 
		//No lo necesitamos
	AddrSpace *aspace;//Which address space is using this page?
	int vpage;//which virtual page number within space?

}fpage_info;

class CoreMap{

  public:

	CoreMap(int nfpages);
	~CoreMap();

	int Find(int vpage,AddrSpace *addr);
	AddrSpace *owner(int fpage);
	int vpn(int fpage);
	void Mark(int fpage,int vpage,AddrSpace *addr);
	int NumClear();
	void Clear(int fpage){bitmap->Clear(fpage);};
	void ClearUp(int fpage,int vpage,AddrSpace *addr);
	void Print();

	int counter;

  private:
	fpage_info *coremap;
	BitMap *bitmap;
	int npages;
};
