#include "..\rt.h"

int main() {

	printf("Creating Child Processes....\n");
	//p1("C:\\RTExamples\\RTExamples\\Q1 - MultiTaskingUsingProcesses\\Q1Child1\\debug\\Q1Child1.exe",	// pathlist to child program executable
	CProcess p1("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_parta\\debug\\Q1Child1.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	CProcess p2("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_parta\\debug\\Q1Child2.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	// child3 int main(int arg1, char * argv[])
	CProcess p3("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_parta\\debug\\Q1Child3.exe fred 1.2",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);


	printf("Child Processes started...\n");

	printf("Waiting for Child 1 to terminate....\n");
	p1.WaitForProcess();
	//printf("Waiting for Child 1 to terminate....\n");
	printf("Waiting for Child 2 to terminate....\n");
	p2.WaitForProcess();

	printf("Waiting for Child 3 to terminate....\n");
	p3.WaitForProcess();

	printf("Done.......");

	return 0;
}