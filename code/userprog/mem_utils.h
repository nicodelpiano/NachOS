void readStrFromUsr(int usrAddr, char *outStr);
void writeStrToUsr(char *str, int usrAddr);
void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);
void writeBuffToUsr(char *buff, int usrAddr, int byteCount);

#define WRITEMEM(d,s,v) if(!machine->WriteMem(d,s,v)) ASSERT(machine->WriteMem(d,s,v))
#define READMEM(d,s,v) if(!machine->ReadMem(d,s,v)) ASSERT(machine->ReadMem(d,s,v))

