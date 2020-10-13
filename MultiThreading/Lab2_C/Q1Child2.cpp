#include "..\rt.h"
#include <stdlib.h> 

//(int argc, char *argv[])
int main(int argc, char* argv[]) {
	int i;
	//for (i = 0; i < 200; i++) {
	//	printf("argc = %d\n", argc);
	//	Sleep(50);
	//}
	printf("argc = %d\n", argc);
	for (i = 0; i < argc; i++) {
		printf("arg[%d] = %s\n", i, argv[i]);
		Sleep(50);
	}
	Sleep(10000);

	//char k=getch() ;


	/*for (x = 0; x < 300; x++) {
		printf("Hello from child process 3....\n");
		Sleep(50);

	}*/
	return 0;
}
