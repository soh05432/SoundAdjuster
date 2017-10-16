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
    /// Interaction logic for CompositionTargetRenderingTest.xaml
    /// </summary>
    public partial class CompositionTargetRenderingTest : Window
    {
        Stopwatch _stopWatch;
        int _frameCounter;
        double _msPerStep;
        double _usedMs;
        bool _reset;

        public PlotModel PlotModel { get; set; }

        public CompositionTargetRenderingTest()
        {
            InitializeComponent();
            this.DataContext = this;
            CompositionTarget.Rendering += this.CompositionTargetRendering;

            // Time check stuff
            this._stopWatch = new Stopwatch();
            this._frameCounter = 0;
            this._msPerStep = 30;
            this._usedMs = 0;
            this._reset = true;

            // Oxyplot stuff
            {
                this.PlotModel = new PlotModel();

                LineSeries dataPoints = new LineSeries
                {
                    LineStyle = LineStyle.Solid
                };
                this.PlotModel.Series.Add(dataPoints);
            }
        }

        void CompositionTargetRendering(object sender, EventArgs e)
        {
            if (_reset)
            {
                _stopWatch.Reset();
                _stopWatch.Start();
                _reset = false;
            }

            // Do things here
            //Thread.Sleep(1);

            _usedMs = _stopWatch.Elapsed.TotalMilliseconds;

            if (_usedMs > _msPerStep)
            {
                // Record time taken
                DataPoint dp = new DataPoint(_frameCounter++, _usedMs);
                var series = (LineSeries)this.PlotModel.Series[0];
                lock (this.PlotModel.SyncRoot)
                {
                    series.Points.Add(dp);
                }
                this.PlotModel.InvalidatePlot(true);

                _reset = true;
            }
        }
    }
}
