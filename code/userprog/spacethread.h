#include "syscall.h"
#include "thread.h"

//definimos la cantidad maxima de procesos ejecutandose
#define MAX_PROCS 100

/*
Aca mantenemos la relacion entre AddrSpaceId y procesos ejecutandose.
Usamos arrays en vez de listas (en la relacion de openfiles e ids usamos listas)
*/

//Estructura para almacenar threads y sus respectivos valores de retorno

struct TRet;

/*typedef struct _TRet;
{
	Thread* t;
	int ret;	
} TRet;

typedef SpaceId AddrSpaceId;
*/
class SpaceThreadTable {

 public:
	SpaceThreadTable();
	~SpaceThreadTable();

	SpaceId addThread(Thread* t);
	int deleteThread(SpaceId id,int ret);
	Thread* getThread(SpaceId id);
	void setRetValue(Thread* t,int status);
	int getRetValue(SpaceId pid);
	SpaceId getPid(Thread *t);

 private:
	TRet ** listaProcs;	

};

