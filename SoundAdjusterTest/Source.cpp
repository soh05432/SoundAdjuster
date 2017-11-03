#include <SoundAdjuster.h>

int main( int argc, char* argv[] )
{
    SA* sa = SA_initialize();

    SA_start( sa );

    //SA_stop( sa );
    
    return 0;
}