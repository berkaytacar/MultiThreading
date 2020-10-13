#include "..\..\rt.h"
#include <stdlib.h> 

int main() {

	printf("Creating Child Processes....\n");
	//p1("C:\\RTExamples\\RTExamples\\Q1 - MultiTaskingUsingProcesses\\Q1Child1\\debug\\Q1Child1.exe",	// pathlist to child program executable
	CProcess p1("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partc\\lab2_partc\\debug\\Q1Child1.exe 5 man utd 7 6.1" ,	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window	
		//PARENT_WINDOW,
		//SUSPENDED
		ACTIVE							// process is active immediately
	);

	CProcess p2("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partc\\lab2_partc\\debug\\Q1Child2.exe hey i am a bot",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window	
		//PARENT_WINDOW,
		//SUSPENDED
		ACTIVE							// process is active immediately
	);

	//// child3 int main(int arg1, char * argv[])
	CProcess p3("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partc\\lab2_partc\\debug\\Q1Child3.exe fred 5.6",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window		
		//PARENT_WINDOW,
		//SUSPENDED
		ACTIVE							// process is active immediately
	);


	printf("Child Processes started...\n");

	printf("Waiting for processes to terminate....\n");
	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();
	
	//printf("Waiting for Child 1 to terminate....\n"); // CTRL KC and CTRL KU to undo
	//p1.WaitForProcess();
	//printf("Waiting for Child 1 to terminate....\n");
	//printf("Waiting for Child 2 to terminate....\n");
	//p2.WaitForProcess();

	//printf("waiting for child 3 to terminate....\n");
	//p3.WaitForProcess();

	
	


	printf("Done.......\n");

	return 0;
}

//printf("Child Processes started in suspended...\n");
	//Sleep(2000); // suspend parent to finish off and start
	//printf("Child Processes started...\n");
	//p1.Suspend();

	//Sleep(2000); // suspend parent to finish off and start
	//printf("Parent Sleep Done..\n");
	//p1.Resume();
	//p3.Resume();
	//printf("Child Process 1 and 3 continues...\n");
	//
	//printf("Child Process 1 and 3 suspended...\n");

	//Sleep(2000);
	//p1.Suspend();
	//p3.Suspend();
	//Sleep(2000);

	//p1.Resume();
	//p3.Resume();
	//printf("Child Process 1 and 3 continues...\n");

	//Sleep(2000);
	//p2.Resume();
	//printf("Child Process 2 continues...\n");