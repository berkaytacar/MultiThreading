#include "..\rt.h"

//(int argc, char *argv[])
int main(int argc, char * argv[]) {
	int x;

	for (x = 0; x < 300; x++) {
		printf("Hello from child process 3....\n");
		Sleep(50);

	}
	return 0;
}