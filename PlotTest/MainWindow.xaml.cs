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
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Diagnostics;
using System.ComponentModel;
using System.Threading;

using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace PlotTest
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // Explore ways to update audio and plot on a constant interval

        public MainWindow()
        {
            InitializeComponent();
        }

        private void BackgroundWorkerMethodClick(object sender, RoutedEventArgs e)
        {
            new BackgroundWorkerTest().Show();
        }

        private void CompositionTargetRenderingMethodClick(object sender, RoutedEventArgs e)
        {
            new CompositionTargetRenderingTest().Show();
        }

        private void TimerMethodClick(object sender, RoutedEventArgs e)
        {
            new TimerTest().Show();
        }
    }
}
