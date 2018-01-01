using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SoundAdjusterApp.Model
{
    class SoundAdjuster
    {
        SoundAdjusterWrapper _impl;

        public SoundAdjuster()
        {
            _impl = new SoundAdjusterWrapper();
        }

        public void Start()
        {
            _impl.start();
        }

        public void Stop()
        {
            _impl.stop();
        }

        public void SetTargetVolume(float value)
        {
            _impl.setTargetVolume(value);
        }

        public void GetAverage(float average) // Not sure if this works now
        {
            _impl.getAverage(ref average);
        }

        public void TurnOnAdjust()
        {
            _impl.turnOnAdjust();
        }

        public void TurnOffAdjust()
        {
            _impl.turnOffAdjust();
        }
    }
}
