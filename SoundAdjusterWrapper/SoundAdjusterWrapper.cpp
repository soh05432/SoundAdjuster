#include "SoundAdjuster.h"

public ref class SoundAdjusterWrapper
{
private:

    SoundAdjuster* m_soundAdjuster;

public:

    SoundAdjusterWrapper()
    {
        m_soundAdjuster = new SoundAdjuster;
    }

    ~SoundAdjusterWrapper()
    {
        delete m_soundAdjuster;
    }

    void start()
    {
        m_soundAdjuster->start();
    }

    void stop()
    {
        m_soundAdjuster->stop();
    }

    void setTargetVolume( float value )
    {
        m_soundAdjuster->setTargetVolume( value );
    }

    float getTargetVolume()
    {
        return m_soundAdjuster->getTargetVolume();
    }

    float getAverage()
    {
        return m_soundAdjuster->getAverage();
    }
};
