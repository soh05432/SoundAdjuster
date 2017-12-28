using System;
using System.Collections.Generic;
using System.Windows;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;
using System.Windows.Input;

using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace SoundAdjusterApp
{
    public class MainViewModel
    {
        private SoundAdjusterWrapper _soundAdjuster;

        public AdjustCommand _AdjustCommand
        {
            get; private set;
        }

        public StopCommand _StopCommand
        {
            get; private set;
        }

        public PlotModel _plotModel { get; set; }

        bool _pressed;

        // Plotting stuff
        DateTime _timeStart;
        int _numPoints;
        int _nextIdx;
        List<DataPoint> _data;

        public MainViewModel()
        {
            // Set up sound adjuster
            _soundAdjuster = new SoundAdjusterWrapper();
            _soundAdjuster.start();

            // Set up plot model
            InitPlotModel();

            _pressed = false;

            // Plotting stuff
            _timeStart = DateTime.Now;
            _numPoints = 250; // 10 seconds / 20 ms = 500
            _nextIdx = 0;
            _data = new List<DataPoint>();

            Timer timer = new Timer( onTimerElapsed );
            timer.Change( 0, 29 );
        }

        ~MainViewModel()
        {
            _soundAdjuster.Dispose();
        }

        private void InitPlotModel()
        {
            this._plotModel = new PlotModel();
            this._plotModel.Background = OxyColors.Black;
            this._plotModel.PlotAreaBorderColor = OxyColors.White;

            LogarithmicAxis yAxis = new LogarithmicAxis
            {
                TextColor = OxyColors.White,
                Position = AxisPosition.Left,
                Base = 10,
                Maximum = 1,
                Minimum = 0.00001, // peakVal * 10 ^ (dB / 20)
                MajorGridlineStyle = LineStyle.Dot,
                MajorGridlineColor = OxyColors.White,
            };

            // Window of 10 seconds
            LinearAxis xAxis = new LinearAxis
            {
                TextColor = OxyColors.White,
                Position = AxisPosition.Bottom,
                MajorGridlineStyle = LineStyle.Dot,
                MajorGridlineColor = OxyColors.White,
            };

            this._plotModel.Axes.Add( yAxis );
            this._plotModel.Axes.Add( xAxis );

            LineSeries data = new LineSeries
            {
                LineStyle = LineStyle.Solid,
                Color = OxyColors.Red,
                StrokeThickness = 1
            };
            this._plotModel.Series.Add( data );

            this._plotModel.MouseDown += ( t, e ) =>
            {
                double y = yAxis.InverseTransform( e.Position.Y );

                if ( _soundAdjuster != null )
                {
                    _soundAdjuster.setTargetVolume( (float)y );
                }

                _pressed = true;
            };

            this._plotModel.MouseMove += ( t, e ) =>
            {
                if ( _pressed )
                {
                    double y = yAxis.InverseTransform( e.Position.Y );

                    if ( _soundAdjuster != null )
                    {
                        _soundAdjuster.setTargetVolume( (float)y );
                    }
                }
            };

            this._plotModel.MouseUp += ( t, e ) =>
            {
                _pressed = false;
            };
        }

        private void Update()
        {
            var s = (LineSeries)_plotModel.Series[0];

            if ( _soundAdjuster != null )
            {
                float average = 0;
                if ( !_soundAdjuster.getAverage( ref average ) )
                {
                    double timeInSeconds = ( DateTime.Now - _timeStart ).TotalMilliseconds / 1000;
                    DataPoint dp = new DataPoint( timeInSeconds, average );
                    UpdatePlotBuffer( dp );
                }
            }
        }

        private void UpdatePlotBuffer( DataPoint point )
        {
            var a = (LinearAxis)_plotModel.Axes[1];
            a.Maximum = Math.Max( a.Maximum, point.X );
            a.Minimum = Math.Max( 0, a.Maximum - 10 );

            var s = (LineSeries)_plotModel.Series[0];

            if ( _data.Count < _numPoints )
            {
                _data.Add( point );

                s.Points.Clear();
                s.Points.AddRange( _data.GetRange( 0, _data.Count ) );
            }
            else
            {
                _data[_nextIdx] = point;
                _nextIdx = ( _nextIdx + 1 ) % _numPoints;

                s.Points.Clear();
                s.Points.AddRange( _data.GetRange( _nextIdx, _numPoints - _nextIdx ) );
                s.Points.AddRange( _data.GetRange( 0, _nextIdx ) );
            }
        }

        private void onTimerElapsed( object state )
        {
            lock ( this._plotModel.SyncRoot )
            {
                this.Update();
            }
            this._plotModel.InvalidatePlot( true );
        }

        public void SetTargetVolume( double value )
        {
            if ( _soundAdjuster != null )
            {
                _soundAdjuster.setTargetVolume( (float)value );
            }
        }

        public class AdjustCommand : ICommand
        {
            public event EventHandler CanExecuteChanged;

            public bool CanExecute(object parameter)
            {
                return true;
            }

            public void Execute(object parameter)
            {
                _soundAdjuster.adjust();
            }
        }

        public class StopCommand : ICommand
        {
            public event EventHandler CanExecuteChanged;

            public bool CanExecute(object parameter)
            {
                return true;
            }

            public void Execute(object parameter)
            {

            }
        }
    }
}
