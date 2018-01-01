using System;
using System.Collections.Generic;


using SoundAdjusterApp.Model;
using SoundAdjusterApp.Helpers;

namespace SoundAdjusterApp
{
    public class MainViewModel
    {
        private SoundAdjuster _soundAdjuster;

        private RtPlot _rtPlot;

        public RelayCommand OnCommand { get; private set; }

        public RelayCommand OffCommand { get; private set; }


        public MainViewModel()
        {
            OnCommand = new RelayCommand(TurnOnAdjust);
            OffCommand = new RelayCommand(TurnOffAdjust);

            _rtPlot = new RtPlot();

            // Set up sound adjuster
            _soundAdjuster = new SoundAdjuster();
            _soundAdjuster.Start();

        }

        ~MainViewModel()
        {
            _soundAdjuster.Dispose();
        }

        public void SetTargetVolume( double value )
        {
            if ( _soundAdjuster != null )
            {
                _soundAdjuster.SetTargetVolume( (float)value );
            }
        }

        private void TurnOnAdjust(object parameter)
        {
            _soundAdjuster.TurnOnAdjust();
        }

        private void TurnOffAdjust(object paraemter)
        {
            _soundAdjuster.TurnOffAdjust();
        }
    }
}
