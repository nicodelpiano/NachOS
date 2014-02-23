//PUERTO
//puerto.h
// clase “Puerto”, que permite que los emisores se sincronicen con los receptores. 
// Puerto::Send(int mensaje) espera atómicamente hasta que se llama a 
// Puerto::Receive(int *mensaje), y luego copia el mensaje int en el buffer de
// Receive. Una vez hecha la copia, ambos pueden retornar. 
// La llamada Receive también es bloqueante (espera a que se ejecute un Send, si no había
// ningún emisor esperando).

#include "synch.h"


class Puerto{

  public:
	Puerto(char *debugName); //constructor
	~Puerto(); //destructor

	void send(int mensaje); //enviamos un mensaje
	void receive(int *mensaje); //recibimos

  private:
	// ponemos los datos privados para asegurarnos de que sea reentrante
        // es una de las condiciones para que sea thread-safe

	int buffer; //buffer
	char *name; //nombre
	Lock *lock; //lock general

        /* reentrante/thread-safe */
	Lock *lockS; //lock de emisores
	Lock *lockR; //lock de receptores
	
        /* variables de condicion */
        Condition *c_s; //si se envia algo
	Condition *c_r; //si se recibe algo
	
	int enviado; // =1 se envio, =0 no se envia todavia (puede interpretarse como lei = 0 y no lei = 1)

};
