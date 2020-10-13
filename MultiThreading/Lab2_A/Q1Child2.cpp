#include "..\rt.h"

int main() {
	int x;

	for (x = 0; x < 250; x++) {
		printf("Hello from child process 2....\n");
		Sleep(50);

	}
	return 0;
}