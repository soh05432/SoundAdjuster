typedef struct IntStruct IS;

IS* initializeIS();
void countUpIS( IS* is ); // Green-lined because intellisense parser different from actual compiler's..

struct IntStruct
{
    int i;
};

IntStruct* initializeIS()
{
    IntStruct* is = new IntStruct;
    is->i = 0;
    return is;
}

void countUpIS( IntStruct* is )
{
    is->i++;
}

#include <iostream>
using namespace std;

#include <windows.h>
#include <mmdeviceapi.h>

int main( int argc, char* argv[] )
{
    IS* is = initializeIS();
    countUpIS( is );
    cout << is->i << endl;

    return 0;
}