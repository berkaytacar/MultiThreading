#include "..\rt.h"


int main() {
	int x;

	for (x = 0; x < 200; x++) {
		printf("Hello from child process 1....\n");
		Sleep(50);

	}
	return 0;
}