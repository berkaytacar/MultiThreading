//#include 	"C:\\RTExamples\\RTExamples\\rt.h"	
#include <process.h>	// for spawnl and createthread
#include <windows.h>	// for perror and sleep
#include <stdio.h>		// for printf
#include <limits.h>		// for UINT_MAX
#include <conio.h>		// for _kbhit(), getch() and getche()
#include <iostream>
#include <string>
#include<mutex>

#define SUSPENDED			101000		// for threads
#define ACTIVE				101001		// for thread

using namespace std;
condition_variable cond;
mutex orderMu;
int cnt = 1;


struct bigData {
	string s;
	int num;
};

HANDLE	GET_STDOUT()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
}

void MOVE_CURSOR(int x, int y)
{
	COORD	c = { (short)x, (short)y };
	SetConsoleCursorPosition(GET_STDOUT(), c);
}
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


class CThread {
private:
	HANDLE ThreadHandle;
	UINT ThreadID;
public:
	CThread(UINT __stdcall Function(void*),
		BOOL bCreateState = ACTIVE,
		void* ThreadArgs = NULL);
	inline virtual ~CThread() { ::TerminateThread(ThreadHandle, 0); }

	inline HANDLE	GetThreadHandle() const { return ThreadHandle; }

	inline UINT	GetThreadID() const { return ThreadID; }

	inline operator HANDLE() const { return ThreadHandle; }

	inline operator UINT()	const { return ThreadID; }

	BOOL Suspend() const;					// can be used to suspend the thread
	BOOL Resume() const;					// can be used to activate a suspended thread
	UINT WaitForThread(DWORD Time = INFINITE) const;			// caller waits for the thread to terminate
	BOOL SetPriority(UINT Priority) const;	// caller sets thread priority, see SET_THREAD_PRORITY() in rt.cpp
	BOOL Post(UINT Message) const;		// caller sends a signal/message to the thread
private:
	void Exit(UINT ExitCode = 0) const;		// called by thread to terminate itself

	inline BOOL TerminateThread(DWORD ExitStatus = 0) { return ::TerminateThread(ThreadHandle, ExitStatus); }

};

CThread::CThread(UINT __stdcall Function(void*), // name/pointer to function that is to be the new thread
	BOOL bCreateState,		// A flag indicating if the thread should commence SUSPENDED (TRUE) or ACTIVE (FALSE)
	void* ThreadArgs		// a generic pointer (can point to anything) to any data the calling thread
						   // wishes to pass to the child thread
)
{
	UINT		ThreadControlFlags = 0;

	if (bCreateState == SUSPENDED)		// if caller wants thread initially suspended
		ThreadControlFlags = CREATE_SUSPENDED;

	ThreadHandle = (HANDLE)(_beginthreadex(NULL, 0, Function, ThreadArgs, ThreadControlFlags, &ThreadID));
	PERR(ThreadHandle != 0, string("Unable to Create Thread"));	// check for error and print message if appropriate
}

//suspend implem
BOOL CThread::Suspend() const
{
	UINT	Result = SuspendThread(ThreadHandle);
	PERR(Result != 0xffffffff, string("Cannot Suspend Thread\n"));	// check for error and print message if appropriate

	if (Result != 0xffffffff)
		return TRUE;
	else
		return FALSE;
}

//resume implem
BOOL CThread::Resume() const
{
	UINT	Result = ResumeThread(ThreadHandle);
	PERR(Result != 0xffffffff, string("Cannot Resume Thread\n"));	// check for error and print message if appropriate

	if (Result != 0xffffffff)
		return TRUE;
	else
		return FALSE;
}

BOOL CThread::SetPriority(UINT Priority) const
{
	BOOL Success;

	// check for error in priority value and print message if appropriate

	PERR(((Priority == THREAD_PRIORITY_ABOVE_NORMAL) ||
		(Priority == THREAD_PRIORITY_BELOW_NORMAL) ||
		(Priority == THREAD_PRIORITY_HIGHEST) ||
		(Priority == THREAD_PRIORITY_IDLE) ||
		(Priority == THREAD_PRIORITY_LOWEST) ||
		(Priority == THREAD_PRIORITY_NORMAL) ||
		(Priority == THREAD_PRIORITY_TIME_CRITICAL)),
		string("Illegal Priority value specified for Thread in call to CThread::SetPriority()"));

	Success = SetThreadPriority(ThreadHandle, Priority);	// set priority

	PERR(Success == TRUE, string("Cannot Set Thread Priority\n"));	// check for error and print message if appropriate

	return Success;
}

UINT CThread::WaitForThread(DWORD Time) const
{
	UINT Result = WaitForSingleObject(ThreadHandle, Time);	// return WAIT_FAILED on error
	PERR(Result != WAIT_FAILED, string("Cannot Wait For Thread"));	// check for error and print error message as appropriate

	return Result;
}

BOOL CThread::Post(UINT Message) const	// message value and ID of thread
{
	BOOL Result;

	if (Message > 32000) {
		PERR(Message <= 32000, string("Could not Post User Message: Message > 32000"));	// check for error and print error message as appropriate
		return FALSE;
	}
	else {
		Result = PostThreadMessage(ThreadID, WM_USER + Message, 0, 0L);
		PERR(Result != 0, string("Could not Post User Message. The Thread might have died"));	// check for error and print error message as appropriate
		return Result;
	}
}

void CThread::Exit(UINT	ExitCode) const
{
	ExitThread(ExitCode);
}







//////// MAIN PART


/*
UINT __stdcall ChildThread (void *args)	// A thread function 
{	
	//int MyThreadNumber = *(int *)(args); 	
	bigData b = *(bigData*)(args);
	unique_lock<mutex> ul(orderMu);

	for (int i = 0; i < 1000; i++) {
		cond.wait(ul, [&]() {return b.num % cnt == 0;});
		std::cout << "I am  Child " << b.s << " thread: My thread number is " << b.num << endl;
		cnt++;
		if (cnt == 9) {
			std::cout << "I am 9 " << endl;
			cnt = 1;
		}
		cond.notify_all();
	}
	return 0 ;
}*/

UINT __stdcall ChildThread(void* args)	// A thread function 
{
	//int MyThreadNumber = *(int *)(args); 	
	bigData b = *(bigData*)(args);
	for (int i = 0; i < 1000; i++) {
		string strr = b.s;
		printf("I am  Child %s thread: My thread number is %d \n", strr.c_str(), b.num);
	}
	return 0;
}

int 	main()
{
	int 	  Num[8] = {0,1,2,3,4,5,6,7} ;   // an array of integers
	string Name[8] = { "A", "B", "C", "D", "F", "G", "H", "I" };
	bigData bd[8];
	

	CThread 	*Threads[8] ;	

//	Now here is the clever bit with threads. Let's create 8 instances of the above
//   thread code and let each thread know which number it is. 

	for ( int i = 0; i < 8; i ++) {
		bd[i].num = i+1;
		bd[i].s = Name[i];
		//cout << "Parent Thread: Creating Child Thread " << bd[i].num << "in Active State" << endl;
		//Threads[ i ] = new CThread (ChildThread, ACTIVE, &Num[i]) ;
		Threads[i] = new CThread(ChildThread, ACTIVE, &bd[i]);
	}
	
	// wait for threads to terminate, then delete thread objects we created above

	for( int j = 0; j < 8; j ++) {
		Threads[ j ] -> WaitForThread() ;	
		delete Threads[ j ] ;	// delete the object created by ‘new’
	}
	return 0 ;
}
