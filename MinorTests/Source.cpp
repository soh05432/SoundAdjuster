#include <thread>
#include <iostream>
#include <windows.h>
#include <future>
#include <functional>

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

void busywait( std::atomic<bool>* run )
{
	while ( *run )
	{

	}
}

void busywaitStopper( std::atomic<bool>* run )
{
	Sleep( 5000 );
	*run = false;
}

int main( int argc, char* argv[] )
{
	//start_thread();

	std::atomic<bool> run = true;
	std::future<void> async_busywaitStopper = std::async( std::launch::async, &busywaitStopper, &run );
	std::future<void> async_busywait = std::async( std::launch::async, &busywait, &run );

	return 0;
}