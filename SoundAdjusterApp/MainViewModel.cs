using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;

using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace SoundAdjusterApp
{
    class MainViewModel
    {
        struct PlotInternalData
        {

        }

        private SoundAdjusterWrapper SoundAdjuster;

        private PlotModel PlotModel;

        // Plotting stuff
        DateTime TimeStart;
        int NumPoints;
        int NextIdx;
        List<DataPoint> Data;

        public MainViewModel()
        {
            NumPoints = 250; // 10 seconds / 20 ms = 500
            NextIdx = 0;
            Data = new List<DataPoint>();

            InitPlotModel();

            SoundAdjuster = new SoundAdjusterWrapper();
            SoundAdjuster.start();
            TimeStart = DateTime.Now;
            Data.Clear();
            NextIdx = 0;

            Timer timer = new Timer(onTimerElapsed);
            timer.Change(0, 29);
        }

        ~MainViewModel()
        {
            SoundAdjuster.Dispose();
        }

        private void InitPlotModel()
        {
            this.PlotModel = new PlotModel();
            this.PlotModel.Background = OxyColors.White;

            LogarithmicAxis yAxis = new LogarithmicAxis
            {
                Position = AxisPosition.Left,
                Base = 10,
                Maximum = 1,
                Minimum = 0.00001, // peakVal * 10 ^ (dB / 20)
            };

            // Window of 10 seconds
            LinearAxis xAxis = new LinearAxis
            {
                Position = AxisPosition.Bottom,
                //MaximumRange = 10,
            };

            this.PlotModel.Axes.Add(yAxis);
            this.PlotModel.Axes.Add(xAxis);

            LineSeries data = new LineSeries
            {
                LineStyle = LineStyle.Solid
            };

            this.PlotModel.Series.Add(data);

            LineSeries optimum = new LineSeries
            {
                LineStyle = LineStyle.Dash
            };

            this.PlotModel.Series.Add(optimum);

            var series = (LineSeries)PlotModel.Series[1];

            this.PlotModel.MouseDown += (t, e) =>
            {
                double y = yAxis.InverseTransform(e.Position.Y);

                if (SoundAdjuster != null)
                {
                    SoundAdjuster.setTargetVolume((float)y);
                }

                series.Points.Clear();
                series.Points.Add(new DataPoint(0, y));
                series.Points.Add(new DataPoint(10, y));
            };
        }
        private void Update()
        {
            var s = (LineSeries)PlotModel.Series[0];

            if (SoundAdjuster != null)
            {
                float average = SoundAdjuster.getAverage();
                double timeInSeconds = (DateTime.Now - TimeStart).TotalMilliseconds / 1000;
                DataPoint dp = new DataPoint(timeInSeconds, average);
                UpdatePlotBuffer(dp);
            }
        }

        private void UpdatePlotBuffer(DataPoint point)
        {
            var s = (LineSeries)PlotModel.Series[0];

            if (Data.Count < NumPoints)
            {
                Data.Add(point);

                s.Points.Clear();
                s.Points.AddRange(Data.GetRange(0, Data.Count));
            }
            else
            {
                Data[NextIdx] = point;
                NextIdx = (NextIdx + 1) % NumPoints;

                s.Points.Clear();
                s.Points.AddRange(Data.GetRange(NextIdx, NumPoints - NextIdx));
                s.Points.AddRange(Data.GetRange(0, NextIdx));
            }

            var a = (LinearAxis)PlotModel.Axes[1];
            a.Maximum = s.Points.Last().X;
            a.Minimum = s.Points.First().X;
        }

        private void onTimerElapsed( object state )
        {
            lock ( this.PlotModel.SyncRoot )
            {
                this.Update();
            }
            
            this.PlotModel.InvalidatePlot( true );
        }

        public void SetTargetVolume(double value)
        {
            if (SoundAdjuster != null)
            {
                SoundAdjuster.setTargetVolume((float)value);
            }
        }
    }
}
