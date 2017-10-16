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
using System.ComponentModel;
using System.Threading;

using OxyPlot;
using OxyPlot.Series;

namespace PlotTest
{
    /// <summary>
    /// Interaction logic for BackgroundWorkerTest.xaml
    /// </summary>
    public partial class BackgroundWorkerTest : Window
    {
        Stopwatch _stopWatch;
        int _frameCounter;
        public PlotModel PlotModel { get; set; }

        public BackgroundWorkerTest()
        {
            InitializeComponent();
            this.DataContext = this;

            // Time check stuff
            this._stopWatch = new Stopwatch();
            this._frameCounter = 0;

            // Oxyplot stuff
            {
                this.PlotModel = new PlotModel();

                LineSeries dataPoints = new LineSeries
                {
                    LineStyle = LineStyle.Solid
                };
                this.PlotModel.Series.Add(dataPoints);
            }

            BackgroundWorker bw = new BackgroundWorker();

            bw.DoWork += (s, e) =>
            {
                const double msPerStep = 30;
                double usedTime = 0;

                while (!bw.CancellationPending)
                {
                    _stopWatch.Reset();
                    _stopWatch.Start();

                    // Do things here
                    Thread.Sleep(5);

                    usedTime = _stopWatch.Elapsed.TotalMilliseconds;

                    // Adjust frame rate
                    if (msPerStep - usedTime > 0)
                    {
                        Thread.Sleep((int)(msPerStep - usedTime));
                    }

                    // Record time taken
                    DataPoint dp = new DataPoint(_frameCounter++, _stopWatch.Elapsed.TotalMilliseconds);
                    var series = (LineSeries)this.PlotModel.Series[0];
                    lock (this.PlotModel.SyncRoot)
                    {
                        series.Points.Add(dp);
                    }
                    this.PlotModel.InvalidatePlot(true);
                }
            };

            bw.RunWorkerAsync();
            this.Closed += (s, e) => bw.CancelAsync();
        }
    }
}
