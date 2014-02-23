#include "spacethread.h"

struct TRet{

	Thread* t;
	int ret;

};

SpaceThreadTable::SpaceThreadTable()
{
	int i;
	listaProcs = new TRet*[MAX_PROCS];
	for(i=0;i<MAX_PROCS;i++)
		listaProcs[i] = NULL;
	
}

SpaceThreadTable::~SpaceThreadTable(){delete listaProcs;};

SpaceId
SpaceThreadTable::addThread(Thread* thread)
{
	int i;
	TRet *temp = new TRet;
	for(i=0;i<MAX_PROCS;i++)
	{
		if(listaProcs[i]==NULL)
		{
			temp->t = thread;
			listaProcs[i] = temp;
			return i;
		}
	
	}
	return -1;
}

int
SpaceThreadTable::deleteThread(SpaceId id,int ret)
{
	listaProcs[id] = NULL;
	return ret;
}

Thread*
SpaceThreadTable::getThread(SpaceId id)
{	
	if(listaProcs[id]!=NULL)
		return(listaProcs[id]->t);
	return NULL;
}

void
SpaceThreadTable::setRetValue(Thread *t,int status)
{
	int i;
	for(i=0;i<MAX_PROCS;i++)
	{
		if(listaProcs[i]->t == t)
		{
			listaProcs[i]->ret = status;
			//DEBUG('j',"stat! %d\n",status);			
			//printf("Ok, %d %d\n",status,i);
			break;
		}
	}
}

SpaceId
SpaceThreadTable::getPid(Thread *t)
{
	int i;
	for(i=0;i<MAX_PROCS;i++)
	{
		if(listaProcs[i]->t == t)
		{
			return i;
		}
	}
	return -1;
}


int
SpaceThreadTable::getRetValue(SpaceId pid)
{
	if(listaProcs[pid]!=NULL)
		return(listaProcs[pid]->ret);
	return -1;
}
