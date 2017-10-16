using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using System.Diagnostics;
using System.Threading;

using OxyPlot;
using OxyPlot.Series;

namespace PlotTest
{
    /// <summary>
    /// Interaction logic for TimerTest.xaml
    /// </summary>
    public partial class TimerTest : Window
    {
        Stopwatch _stopWatch;
        int _frameCounter;

        public PlotModel PlotModel { get; set; }

        public TimerTest()
        {
            InitializeComponent();
            this.DataContext = this;

            // Oxyplot stuff
            {
                this.PlotModel = new PlotModel();

                LineSeries dataPoints = new LineSeries
                {
                    LineStyle = LineStyle.Solid
                };
                this.PlotModel.Series.Add(dataPoints);
            }

            this._stopWatch = new Stopwatch();
            this._frameCounter = 0;
            Timer timer = new Timer(onTimerElapsed);
            timer.Change(0, 29);
        }

        private void onTimerElapsed(object state)
        {
            if (_frameCounter++ > 0)
            {
                // Record time taken
                DataPoint dp = new DataPoint(_frameCounter++, _stopWatch.Elapsed.TotalMilliseconds);
                var series = (LineSeries)this.PlotModel.Series[0];
                lock (this.PlotModel.SyncRoot)
                {
                    series.Points.Add(dp);
                }
                this.PlotModel.InvalidatePlot(true);
            }

            _stopWatch.Reset();
            _stopWatch.Start();
        }
    }
}
