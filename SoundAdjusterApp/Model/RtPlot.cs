using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace SoundAdjusterApp.Model
{
    class RtPlot
    {
        bool _pressed;

        // Plotting stuff
        DateTime _timeStart;
        int _numPoints;
        int _nextIdx;
        List<DataPoint> _data;

        public PlotModel PlotModel { get; set; }

        public RtPlot()
        {
            _pressed = false;

            // Plotting stuff
            _timeStart = DateTime.Now;
            _numPoints = 250; // 10 seconds / 20 ms = 500
            _nextIdx = 0;
            _data = new List<DataPoint>();

            Timer timer = new Timer(onTimerElapsed);
            timer.Change(0, 29);
        }

        InitPlotModel()
        {
            this.PlotModel = new PlotModel();
            this.PlotModel.Background = OxyColors.Black;
            this.PlotModel.PlotAreaBorderColor = OxyColors.White;

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

            this.PlotModel.Axes.Add(yAxis);
            this.PlotModel.Axes.Add(xAxis);

            LineSeries data = new LineSeries
            {
                LineStyle = LineStyle.Solid,
                Color = OxyColors.Red,
                StrokeThickness = 1
            };
            this.PlotModel.Series.Add(data);

            this.PlotModel.MouseDown += (t, e) =>
            {
                double y = yAxis.InverseTransform(e.Position.Y);

                if (_soundAdjuster != null)
                {
                    _soundAdjuster.SetTargetVolume((float)y);
                }

                _pressed = true;
            };

            this.PlotModel.MouseMove += (t, e) =>
            {
                if (_pressed)
                {
                    double y = yAxis.InverseTransform(e.Position.Y);

                    if (_soundAdjuster != null)
                    {
                        _soundAdjuster.SetTargetVolume((float)y);
                    }
                }
            };

            this.PlotModel.MouseUp += (t, e) =>
            {
                _pressed = false;
            };
        }

        private void onTimerElapsed(object state)
        {
            lock (this.PlotModel.SyncRoot)
            {
                this.Update();
            }
            this.PlotModel.InvalidatePlot(true);
        }

        private void Update()
        {
            var s = (LineSeries)PlotModel.Series[0];

            if (_soundAdjuster != null)
            {
                float average = 0;
                if (!_soundAdjuster.GetAverage(ref average))
                {
                    double timeInSeconds = (DateTime.Now - _timeStart).TotalMilliseconds / 1000;
                    DataPoint dp = new DataPoint(timeInSeconds, average);
                    UpdatePlotBuffer(dp);
                }
            }
        }

        private void UpdatePlotBuffer(DataPoint point)
        {
            var a = (LinearAxis)PlotModel.Axes[1];
            a.Maximum = Math.Max(a.Maximum, point.X);
            a.Minimum = Math.Max(0, a.Maximum - 10);

            var s = (LineSeries)PlotModel.Series[0];

            if (_data.Count < _numPoints)
            {
                _data.Add(point);

                s.Points.Clear();
                s.Points.AddRange(_data.GetRange(0, _data.Count));
            }
            else
            {
                _data[_nextIdx] = point;
                _nextIdx = (_nextIdx + 1) % _numPoints;

                s.Points.Clear();
                s.Points.AddRange(_data.GetRange(_nextIdx, _numPoints - _nextIdx));
                s.Points.AddRange(_data.GetRange(0, _nextIdx));
            }
        }
    }
}
