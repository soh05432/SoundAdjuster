#include <thread>
#include <iostream>
#include <windows.h>

using namespace std;

int thread_func()
{
	int i = 0;

	while ( true )
	{
		cout << i++ << endl;
	}
}

int start_thread()
{
	thread t( thread_func );

	Sleep( 1000 * 5 );
	//t.detach();

	return 0;
}

int main( int argc, char* argv[] )
{
	start_thread();

	return 0;
}