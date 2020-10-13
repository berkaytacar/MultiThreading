#include 	"..\..\rt.h"
#include <iostream>
#include <string>
#include <stdio.h>

typedef struct bigData {
    string s;
    int num;
}bigData;


//int main()
//{
//    using namespace std;
//
//    string myString = "Press ENTER to quit program!";
//    cout << "Come up and C++ me some time." << endl;
//    printf("Follow this command: %s", myString.c_str()); //note the use of c_str

UINT __stdcall ChildThread(void* args)    // A thread function 
{
    //int MyThreadNumber = *(int *)(args);
    bigData b = *(bigData*)(args);
    using namespace std;
    for (int i = 0; i < 10000; i++) {
        //Sleep(50);
        //cout << "I am  Child" << b.s << " thread: My thread number is " << b.num <<"\n"<< endl;
        string strr = b.s;
        printf("I am  Child %s thread: My thread number is %d \n", strr.c_str(), b.num);
        //printf("I am  Child thread: My thread number is %d \n", b.num);
        //Sleep(50);
    }
    return 0;
}

int     main()
{
    int       Num[8] = { 0,1,2,3,4,5,6,7 };   // an array of integers
    bigData bd[8];
    

    CThread* Threads[8];

    //    Now here is the clever bit with threads. Let's create 8 instances of the above
    //   thread code and let each thread know which number it is. 

    for (int i = 0; i < 8; i++) {
        //bd[i].num = 0;
        //bd[i].s = "A";
        //string Name[8] = { "Mo", "Rashford", "Sad", "Martial", "Dijk", "Robbo", "TAA", "Bruna" };
        string Name[8] = { "A", "B", "C", "D", "E", "F", "G", "H" };
        bd[i].num = i;
        bd[i].s = Name[i];
        //cout << "Parent Thread: Creating Child Thread " << i << "in Active State\n";
        printf("Parent Thread: Creating Child Thread %d in Active State\n", i);
        //Threads[ i ] = new CThread (ChildThread, ACTIVE, &Num[i]) ;
        Threads[i] = new CThread(ChildThread, ACTIVE, &bd[i]);
        Sleep(50);
    }

    // wait for threads to terminate, then delete thread objects we created above

    for (int j = 0; j < 8; j++) {
        Threads[j]->WaitForThread();
        delete Threads[j];    // delete the object created by ‘new’
    }
    return 0;
}

//UINT __stdcall ChildThread (void *args)	// A thread function 
//{	
//	int MyThreadNumber = *(int *)(args); 	
//	
//	for ( int i = 0; i < 10000; i ++)
//		cout << "I am the Child thread: My thread number is " << MyThreadNumber << endl;
//
//	return 0 ;
//}
//
//int 	main()
//{
//	int 	  Num[8] = {0,1,2,3,4,5,6,7} ;   // an array of integers
//	
//	printf("Yoo");
//
//	CThread 	*Threads[8] ;	
//
////	Now here is the clever bit with threads. Let's create 8 instances of the above
////   thread code and let each thread know which number it is. 
//
//	for ( int i = 0; i < 8; i ++) {
//		//cout << "Parent Thread: Creating Child Thread " << i << "in Active State\n";
//		printf("Parent Thread: Creating Child Thread %d \n", i);
//		Threads[ i ] = new CThread (ChildThread, ACTIVE, &Num[i]) ;
//	}
//	
//	// wait for threads to terminate, then delete thread objects we created above
//
//	for( int j = 0; j < 8; j ++) {
//		Threads[ j ] -> WaitForThread() ;	
//		delete Threads[ j ] ;	// delete the object created by ‘new’
//	}
//	return 0 ;
//}
