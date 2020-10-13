//#include 	"C:\\RTExamples\\RTExamples\\rt.h"	
#include <process.h>	// for spawnl and createthread
#include <windows.h>	// for perror and sleep
#include <stdio.h>		// for printf
#include <limits.h>		// for UINT_MAX
#include <conio.h>		// for _kbhit(), getch() and getche()
#include <iostream>
#include <string>
#include<mutex>

#include<thread>

using namespace std;

struct bigData {
	string s;
	int num;
};

//////// MAIN PART
int ChildThread(int x, string s)	// A thread function 
{
	for (int i = 0; i < 1000; i++) {
		string strr = s;
		printf("I am  Child %s thread: My thread number is %d \n", strr.c_str(), x);
	}
	return 0;
}
int 	main()
{
	int Num[8] = {0,1,2,3,4,5,6,7} ;   // an array of integers
	string Name[8] = { "A", "B", "C", "D", "F", "G", "H", "I" };
	bigData bd[8];

	thread *Threads[8] ;	

//	Now here is the clever bit with threads. Let's create 8 instances of the above
//   thread code and let each thread know which number it is. 

	for ( int i = 0; i < 8; i ++) {
		bd[i].num = i+1;
		bd[i].s = Name[i];
		//cout << "Parent Thread: Creating Child Thread " << bd[i].num << "in Active State" << endl;
		//Threads[ i ] = new CThread (ChildThread, ACTIVE, &Num[i]) ;
		Threads[i] = new thread(ChildThread, bd[i].num, bd[i].s);
	}
	
	// wait for threads to terminate, then delete thread objects we created above

	for( int j = 0; j < 8; j ++) {
		Threads[ j ] -> join() ;	
		delete Threads[ j ] ;	// delete the object created by ‘new’
	}
	return 0 ;
}
