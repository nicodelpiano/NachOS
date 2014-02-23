#include "syscall.h"

int main(int argc, char **argv) {
	int i,j;
	char nl = '\n';

	for(i=0; i<argc; i++) {
		for(j = 0; argv[i][j] != 0; j++)
			Write(argv[i] + j, 1, 1);

		Write(&nl, 1, 1);
	}

	Exit(666);
}
