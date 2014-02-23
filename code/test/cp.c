#include "syscall.h"

int main(int argc,char **argv)
{
	int i;
	char c;
	if(argc!=2){
		Write("cp: error, argumentos insuficientes o excedidos.\n",49,ConsoleOutput); 
		return;
	}	
	
	OpenFileId fd = Open(argv[1]);//en argv[1] esta el archivo a copiar
	//Create(argv[0]);//creamos el archivo
	OpenFileId fd2 = Open(argv[0]);//lo abrimos
	if(fd2==-1)
	{
		Create(argv[0]);
		fd2 = Open(argv[0]);
	}
	if(fd!=-1 && fd2!=-1){
		while(Read(&c,1,fd)){
			Write(&c,1,fd2);
			//Write("a",1,ConsoleOutput);
		}	
		Close(fd);
		Close(fd2);
	
	}else{
		Write("cp: error, no se pudo encontrar el archivo.\n",45,ConsoleOutput);
	}
	
}
