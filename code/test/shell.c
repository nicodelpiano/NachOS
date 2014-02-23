#include "syscall.h"
#define BUFF_LEN 128
#define MAX_ARGS 30

//vamos a parsear el buffer
//para hacer exec correctamente
int parseBuff(char *buff, char *args[])
{
	int argc=0;
	int i;
	char *c = buff;
	args[argc] = c;

	while(*c!='\0')
	{
		while(*c!='\0' && *c!=' ')c++;
		if(*c == ' '){
			*c = '\0';
			*(c++);
			argc++;
			args[argc] = c;
		}
	}
	
	return argc;
}


int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[BUFF_LEN];
    int counter=0;
	char *args[MAX_ARGS];
	int i,j;
	char *argsA[MAX_ARGS];

    prompt[0] = '$';
	prompt[1] = ' ';
    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	
	do {
	
	    Read(&buffer[i], 1, input); 

	} while( buffer[i++] != '\n' );

	buffer[--i] = '\0';
	
	if(i>0){
		if(buffer[0]=='&'){
			int argc = parseBuff(&buffer[1],args);
	
			//for(j=0;j<argc;j++)
			//	argsA[j] = args[j+1];
			newProc = Exec(args[0],argc,&args[1]);
		}else{
			int argc = parseBuff(buffer,args);
	
			//for(j=0;j<argc;j++)
			//	argsA[j] = args[j+1];
			newProc = Exec(args[0],argc,&args[1]);
			Join(newProc);
		
    		}
	}

//Exec("test",0,"");
}
}
