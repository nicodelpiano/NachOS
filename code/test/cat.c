#include "syscall.h"

int main(int argc,char **argv)
{
	int i;
	char c;
	if(argc==0)
	{
		while(1){
			if(Read(&c,1,ConsoleInput)<0)break;			
			Write(&c,1,ConsoleOutput);
		}
	}else if(argc>0){
		for(i=0;i<argc;i++){
			OpenFileId fd = Open(argv[i]);
			if(fd!=-1){
				while(Read(&c,1,fd)){
					Write(&c,1,ConsoleOutput);
				}
				Close(fd);
			}else{
				Write("cat: error, no se pudo encontrar el archivo.\n",45,1);
			}
		}
	}

}

