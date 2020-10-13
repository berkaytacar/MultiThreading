

#include <process.h>	// for spawnl and createthread
#include <windows.h>	// for perror and sleep
#include <stdio.h>		// for printf
#include <limits.h>		// for UINT_MAX
#include <conio.h>		// for _kbhit(), getch() and getche()
#include <iostream>
#include <string>

using namespace std;

#define PerThreadStorage  __declspec(thread)
#define _CRT_SECURE_NO_WARNINGS	


#define SUSPENDED			101000		// for threads
#define ACTIVE				101001		// for thread
#define OWN_WINDOW			101002		// for FORK_PROCESS
#define PARENT_WINDOW		101003		// ditto
#define MAXMESSAGEVALUE		32000		// maximum value of a user generated message

#ifdef __BORLANDC__						// if the compiler is Borand C++
#define _P_NOWAIT		P_NOWAIT	// For Borland Builder Compatibility
#define	_WAIT_CHILD		WAIT_CHILD
#endif									// end Borland C++ section

// data structure related to datapools

//##ModelId=3DE6123902E9
typedef struct {
	HANDLE DataPoolHandle;		// handle to datapool
	void* DataPoolPointer;		// pointer that can be used to access datapool
}DATAPOOLINFO;

void PERR(bool bSuccess, string ErrorMessageString);

void	SLEEP(UINT	Time);			// suspend current thread for 'Time' mSec
BOOL	TEST_FOR_KEYBOARD();		// tests a keyboard for a key press returns true if key pressed
HANDLE	GET_STDIN();				// get handle to standard input device (keyboard)
HANDLE	GET_STDOUT();				// ditto output device
HANDLE	GET_STDERR();				// ditto erro device
UINT	WAIT_FOR_CONSOLE_INPUT(HANDLE hEvent, DWORD Time = INFINITE);	//wait for console input to happen


void	MOVE_CURSOR(int x, int y);	// move console cursor to x,y coord
void	CURSOR_ON();				// turn flashing cursor on (the default)
void	CURSOR_OFF();				// turn flashing cursor off
void	REVERSE_ON();				// turn on inverse video
void	REVERSE_OFF();				// turn off inverse video


UINT	WAIT_FOR_MULTIPLE_OBJECTS(UINT nCount,             // number of handles in the handle array
	CONST HANDLE* lpHandles, // pointer to the object-handle array
	DWORD Time = INFINITE
);

// Cprocess Class Declaration Here
//##ModelId=3DE612390389
class CProcess {					// see Process related functions in rt.cpp for more details
private:
	PROCESS_INFORMATION pInfo;
	const string ProcessName;		// name of process

public:

	CProcess(const string& Name,							// pathlist/name of executable program (.exe) that will be the child process
		int Priority = NORMAL_PRIORITY_CLASS,		// Priority of the process
													//	The allowed priorities are:	
													//	HIGH_PRIORITY_CLASS			Use for important time critical tasks
													//	IDLE_PRIORITY_CLASS			Use for processes that run when all other processes are idle
													//	NORMAL_PRIORITY_CLASS		Normal processing shared with other normal processes
													//	REALTIME_PRIORITY_CLASS		highest priority blocks out all other processes including O.System so take care

		BOOL bUseNewWindow = PARENT_WINDOW,			// use OWN_WINDOW to make new process use its own windows
													// use PARENT_WINDOW to make it share its parent window

		BOOL bCreateSuspended = ACTIVE,				// use SUSPENDED to make new child processes main thread suspended when process is created
													// use ACTIVE to make new child processes main thread active when process iscreated
		const string& ChildProcessArgString = ""
	);

	virtual ~CProcess() {}				// destructor (does nothing by default)
	inline operator string	() const { return ProcessName; }
	inline string	GetName() const { return ProcessName; }

	inline HANDLE	GetProcessHandle() const { return pInfo.hProcess; }		// get the process's handle
	inline HANDLE	GetThreadHandle() const { return pInfo.hThread; }		// get the process's main thread handle
	inline UINT		GetProcessId() const { return pInfo.dwProcessId; }		// get the process's ID
	inline UINT		GetThreadId() const { return pInfo.dwThreadId; }		// get the main thread's ID

	inline operator PROCESS_INFORMATION() const { return pInfo; }	// get general info about process

	BOOL Suspend() const;										// called to suspend the child process i.e. pause it
	BOOL Resume() const;										// allows child to resume processing
	BOOL WaitForProcess(DWORD Time = INFINITE) const;			// allows parent to wait for child child process to finish
	BOOL SetPriority(int Priority) const;						// changes the priority of the child process (see allowed process values in constructor)
	BOOL Post(UINT Message) const;							// allows a signal/message to be sent to the process

private:
	void Exit(UINT ExitCode) const;		// called by process to terminate itself
	inline BOOL TerminateProcess(UINT ExitStatus = 0) { return ::TerminateProcess(GetProcessHandle(), ExitStatus); }
};

//////////////////////////// CProcess Constructor /////////////////////////////////////////////
CProcess::CProcess(
	const string& Name,		// path/name of executable program (.exe)
	int Priority,			// Priority of the process
	BOOL bUseNewWindow,		// use OWN_WINDOW to make new process run in its own window
							// use PARENT_WINDOW to make it share its parent window
	BOOL bCreateSuspended,	// use SUSPENDED to make new child process's main thread suspended when process is created
							// use ACTIVE to make new child processes main thread active when process iscreated

	const string& ChildProcessArgString)

	: ProcessName(Name)
{
	// Check priority level is valid



	PERR(((Priority == HIGH_PRIORITY_CLASS) ||
		(Priority == IDLE_PRIORITY_CLASS) ||
		(Priority == NORMAL_PRIORITY_CLASS) ||
		(Priority == REALTIME_PRIORITY_CLASS)),
		string("Illegal 2nd Argument (Process Priority) for Process: ") + Name);


	PERR((bUseNewWindow == OWN_WINDOW || bUseNewWindow == PARENT_WINDOW),
		string("Use OWN_WINDOW or PARENT_WINDOW as 3rd argument for Process: ") + Name);
	PERR((bCreateSuspended == SUSPENDED || bCreateSuspended == ACTIVE),
		string("Use SUSPENDED or ACTIVE as 4th Argument for Process: ") + Name);


	STARTUPINFO	StartupInfo = {
		sizeof(STARTUPINFO) ,
		NULL ,			// reserved
		NULL ,			// ignored in console applications
		(char*)(Name.c_str()) ,	// displayed in title bar for console applications
		0,0,			// dwx, dwy, offset of top left of new window relative to top left of screen in pixel
						// flags below must specify STARTF_USEPOSITION. Ignored for console apps'
		0,0,			// dwxsize, dwysize: Width and height of the window if new window specified
						// must use flags STARTF_USESIZE. Ignored for console apps'
		0,0,			// size of console in characters, only if STARTF_USECOUNTCHARS flag specified, 
						// Ignored for console apps
		0,				// Colour control, for background and text. Ignored for console apps
		0,				// Flags. Ignored for console applications
		0,				// ignored unless showwindow flag set
		0 ,
		NULL,
		0,0,0			// stdin, stdout and stderr handles (inherited from parent)
	};

	UINT flags = Priority; // Priority,

	if (bUseNewWindow == OWN_WINDOW)		// if caller has specified that child should have its won window
		flags |= CREATE_NEW_CONSOLE;

	if (bCreateSuspended == SUSPENDED)	// if caller has specified that child process should be immediately suspended
		flags |= CREATE_SUSPENDED;

	BOOL Success = CreateProcess(NULL,	// application name
		(char*)(Name.c_str()),			// Command line to the process if you want to pass one to main() in the process
		NULL,			// process attributes
		NULL,			// thread attributes
		TRUE,			// inherits handles of parent
		flags,			// Priority and Window control flags,
		NULL,			// use environent of parent
		NULL,			// use same drive and directory as parent
		&StartupInfo,	// controls appearance of process (see above)
		&pInfo			// Stored process handle and ID into this object
	);
	PERR(Success == TRUE, string("CProcess Call Unable to Create New Process: ") + Name);	// check for error and print message if appropriate
}
///// PERR HERE //////

void PERR(bool bSuccess, string ErrorMessageString)
{
	UINT LastError = GetLastError();

	if (!(bSuccess)) {
		char buff[512];
		Beep(500, 100);
		MOVE_CURSOR(0, 0);
		REVERSE_ON();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, LastError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buff, 1024, NULL);
		printf(" Error %d in Process %s: line %d:\n", LastError, __FILE__, __LINE__);
		printf(" Translation: %s Error: %s", buff, ErrorMessageString.c_str());
		REVERSE_OFF();
		printf("\n\nPress Return to Continue...");
		_getch();
	}
}

void	SLEEP(UINT	Time)
{
	Sleep(Time);
}

//
//	This function tests the keyboard to see if a key has been pressed. If so, the value TRUE is
//	returned and the thread can read the character without getting suspended using a function
//	such as _getch(), _getche() etc.
//

BOOL	TEST_FOR_KEYBOARD()
{
	return _kbhit();
}


//
//	These functions returns handles to a console standard input/output and error device
//	i.e. keyboard, screen and screen respectively. NULL is returned on error
//	For more example of console functions read the Vis C++ Help/Win32 functions by category/console functions
//

HANDLE	GET_STDIN()
{
	return GetStdHandle(STD_INPUT_HANDLE);
}

HANDLE	GET_STDOUT()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
}

HANDLE	GET_STDERR()
{
	return GetStdHandle(STD_ERROR_HANDLE);
}

//
//	This function pauses the thread/process until user input is entered at the keyboard
//
//	This function returns WAIT_FAILED if there was an error, WAIT_TIMEOUT if the wait operation timed out
//	of WAIT_OBJECT_0 if the operation did in fact wait and then returned
//

UINT WAIT_FOR_CONSOLE_INPUT(HANDLE hEvent, DWORD Time)
{
	UINT	Status = WaitForSingleObject(hEvent, Time);
	PERR(Status != WAIT_FAILED, string("Cannot Wait for Console Input"));	// check for error and print message if appropriate
	return Status;
}

//	moves the cursor to the x,y coord on the screen. [0,0] is top left
//	all calls to printf cause output to occur at the current cursor position
//	obviously, the cursor moves with text output operations

void MOVE_CURSOR(int x, int y)
{
	COORD	c = { (short)x, (short)y };
	SetConsoleCursorPosition(GET_STDOUT(), c);
}

//
//	These two function turns off/on the cursor so that it is not visible
//	but it still operates and output text still occurs at the current curso
//	position
//

void CURSOR_OFF()
{
	CONSOLE_CURSOR_INFO	cci = { 1, FALSE };
	SetConsoleCursorInfo(GET_STDOUT(), &cci);
}

void CURSOR_ON()
{
	CONSOLE_CURSOR_INFO	cci = { 1, TRUE };
	SetConsoleCursorInfo(GET_STDOUT(), &cci);
}

//
//	These two functions turns on/off reverse video, so that text comes black on white background
//	For more details set the SetConsoleTextAttribute() function in on-line
//	help
//

void REVERSE_ON()
{
	SetConsoleTextAttribute(GET_STDOUT(),
		BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
}

void REVERSE_OFF()
{
	SetConsoleTextAttribute(GET_STDOUT(),
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void CLEAR_SCREEN()
{
	for (int i = 0; i < 50; i++)
		putchar('\n');
}
// set priority here 
BOOL CProcess::SetPriority(int Priority) const
{
	BOOL Success;

	// Check priority level 

	PERR(((Priority == HIGH_PRIORITY_CLASS) || (Priority == IDLE_PRIORITY_CLASS) ||
		(Priority == NORMAL_PRIORITY_CLASS) || (Priority == REALTIME_PRIORITY_CLASS)),
		string("Illegal Priority value in call to SetPiority()"));

	Success = SetPriorityClass(GetProcessHandle(), Priority);
	PERR(Success == TRUE, string("Unable to Set Thread Priority of Process: ") + ProcessName);	// check for error and print error message as appropriate

	return Success;
}
// CProcess Suspend
//##ModelId=3DE6123A007F
BOOL CProcess::Suspend() const
{
	UINT	Result = SuspendThread(GetThreadHandle());
	PERR(Result != 0xffffffff, string("Cannot Suspend Process: ") + ProcessName);

	if (Result != 0xffffffff)	// if no error
		return TRUE;
	else
		return FALSE;
}
// Resume here 
//##ModelId=3DE6123A0088
BOOL CProcess::Resume() const
{
	UINT	Result = ResumeThread(GetThreadHandle());
	PERR(Result != 0xffffffff, string("Cannot Resume Process: ") + ProcessName);

	if (Result != 0xffffffff)	// if no error
		return TRUE;
	else
		return FALSE;
}

// WaitForProcess HERE //////
//##ModelId=3DE6123A0092
BOOL	CProcess::WaitForProcess(DWORD Time) const
{
	UINT Result = WaitForSingleObject(GetProcessHandle(), Time);
	PERR(Result != WAIT_FAILED, string("Cannot Wait for Child Process: ") + ProcessName + string(" to Terminate.\n It might already be dead"));	// check for error and print message if appropriate== WAIT_OBJECT_0 ) 

	return Result;
}



///////////////// MAIN Code HERE /////////////////////////////////////
int main() {

	printf("Creating Child Processes....\n");
	//p1("C:\\RTExamples\\RTExamples\\Q1 - MultiTaskingUsingProcesses\\Q1Child1\\debug\\Q1Child1.exe",	// pathlist to child program executable
	CProcess p1("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partb\\lab2_partb\\debug\\Q1Child1.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		//OWN_WINDOW,						// process has its own window	
		PARENT_WINDOW,
		SUSPENDED
		//ACTIVE							// process is active immediately
	);

	CProcess p2("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partb\\lab2_partb\\debug\\Q1Child2.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		//OWN_WINDOW,						// process has its own window	
		PARENT_WINDOW,
		SUSPENDED
		//ACTIVE							// process is active immediately
	);

	// child3 int main(int arg1, char * argv[])
	CProcess p3("C:\\RTExamples\\RTExamples\\Q1_mine\\lab2_partb\\lab2_partb\\debug\\Q1Child3.exe fred 1.2",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		//OWN_WINDOW,						// process has its own window		
		PARENT_WINDOW,
		SUSPENDED
		//ACTIVE							// process is active immediately
	);


	//printf("Child Processes started...\n");
	
	//printf("Waiting for Child 1 to terminate....\n"); // CTRL KC and CTRL KU to undo
	//p1.WaitForProcess();
	////printf("Waiting for Child 1 to terminate....\n");
	//printf("Waiting for Child 2 to terminate....\n");
	//p2.WaitForProcess();

	//printf("Waiting for Child 3 to terminate....\n");
	//p3.WaitForProcess();
	printf("Child Processes started in suspended...\n");
	Sleep(2000); // suspend parent to finish off and start
	//printf("Child Processes started...\n");
	//p1.Suspend();
	
	//Sleep(2000); // suspend parent to finish off and start
	printf("Parent Sleep Done..\n");
	p1.Resume();
	p3.Resume();
	printf("Child Process 1 and 3 continues...\n");
	
	printf("Child Process 1 and 3 suspended...\n");

	Sleep(2000);
	p1.Suspend();
	p3.Suspend();
	Sleep(2000);

	p1.Resume();
	p3.Resume();
	printf("Child Process 1 and 3 continues...\n");

	Sleep(2000);
	p2.Resume();
	printf("Child Process 2 continues...\n");


	//printf("Done.......\n");

	return 0;
}