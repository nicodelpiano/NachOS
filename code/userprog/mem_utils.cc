#include "machine.h"
#include "system.h"
#include "mem_utils.h"

void readStrFromUsr(int usrAddr, char *outStr)
{
	int i=0;
	int j;
	READMEM(usrAddr+i,1,&j);
	while(j!='\0')
	{
		
		outStr[i] = j;    
		i++;
		READMEM(usrAddr+i,1,&j);	
	}	
	//printf("lala\n");
	outStr[i]=j;
		
}

void writeStrToUsr(char *str, int usrAddr)
{
	int i=0;

	while(str[i]!=0)
	{
		WRITEMEM(usrAddr+i,1,str[i]);
		i++;
	}

	WRITEMEM(usrAddr+i,1,0);
}

void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount)
{
	int i=0;
	int j;
	
	while(i<byteCount)		
	{
		READMEM(usrAddr+i,1,&j);
		outBuff[i] = j;
		i++;
	}
}

void writeBuffToUsr(char *buff, int usrAddr, int byteCount)
{
	int i;
	for(i=0;i<byteCount;i++)
		WRITEMEM(usrAddr+i,1,buff[i]);
			
	
}
