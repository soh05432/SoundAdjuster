#include "SoundAdjuster.h"

public ref class SoundAdjusterWrapper
{
private:

	SAC* m_sac;

public:

    SoundAdjusterWrapper()
    {
		m_sac = SA_initialize();
    }

    ~SoundAdjusterWrapper()
    {
		SA_stop( m_sac );
    }

    void start()
    {
		SA_start( m_sac );
    }

    void stop()
    {
		SA_stop( m_sac );
    }

    void setTargetVolume( float value )
    {
		SA_setTargetVolume( m_sac, value );
    }

    float getTargetVolume()
    {
		return 0.f;
    }

	bool getAverage( float% res )
    {
		float r = res;
		if ( !SA_getAverage( m_sac, r ) )
		{
			res = r;
			return false;
		}

		return true;
    }

	void adjust()
	{
		SA_setAdjustMode( m_sac, ADJUST );
	}

	void suspend()
	{
		SA_setAdjustMode( m_sac, NO_ADJUST );
	}
};
