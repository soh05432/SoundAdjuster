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

    }

    float getTargetVolume()
    {
		return 0.f;
    }

    float getAverage()
    {
		return 0.f;
    }
};
