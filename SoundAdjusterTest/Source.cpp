#include <SoundAdjuster.h>

#include <windows.h>

int main( int argc, char* argv[] )
{
    SAC* sac = SA_initialize();

    SA_start( sac );

#if 0
	Sleep( 5000 );
	SA_setAdjustMode( sac, ADJUST_MODE::NO_ADJUST );
	Sleep( 5000 );
	SA_setAdjustMode( sac, ADJUST_MODE::ADJUST );
	Sleep( 5000 );
#endif

	Sleep( 10000 );

	//SA_stop( sac );
    
    return 0;
}