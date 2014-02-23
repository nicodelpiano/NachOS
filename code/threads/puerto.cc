//Puerto

#include "synch.h"
#include "puerto.h"

/*
Nuestra implementacion se caracteriza por ser reentrante y thread-safe.
Es decir, el primero que toma el lock (lockS o lockR) va a ser el primero que va a enviar o recibir respectivamente, y asi nos aseguramos que otro no pueda enviar o recibir hasta que no se liberen los locks correspondientes.
*/

Puerto::Puerto(char *debugName){

	name = debugName;
	lock = new Lock("lock");

	//locks para asegurar que sea reentrante
	lockS = new Lock("lockS");
	lockR = new Lock("lockR");

	//variabes de condicion
	c_s  = new Condition("cond_send",lock);
	c_r  = new Condition("cond_recv",lock);
	enviado = 0;
        
}

Puerto::~Puerto(){
	delete lock;
	delete lockS;
	delete lockR;
	delete c_s;
	delete c_r;
}

void Puerto::send(int mensaje){

	lockS->Acquire();//vamos a asegurarnos de ser el unico en enviar algo
	lock->Acquire();
  
	buffer = mensaje;//ponemos el mensaje en el buffer
	
	enviado = 1;//vamos a enviar

	c_s->Signal();//para ello despertamos a un sender
	//if
	while(enviado)//mientras el mensaje todavia no se consumio por algun receptor
		c_r->Wait();//esperamos
	
	lock->Release();
	lockS->Release();
}

void Puerto::receive(int *mensaje){
	
	lockR->Acquire();// nos aseguramos de ser el unico en recibir
	lock->Acquire();
  
	while(!enviado)//nadie envio nada todavia
		c_s->Wait(); //esperamos a que alguien envie

	*mensaje = buffer; //guardamos el mensaje
	
	enviado = 0; //consumimos lo enviado

	c_r->Signal(); //despertamos a un receptor (podriamos hacer un broadcast tambien)
                       //usado para que si quedo un send en wait, se despierte y pueda liberar el lock
	lock->Release();
	lockR->Release();
}
