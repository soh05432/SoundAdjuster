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

using Hardcodet.Wpf.TaskbarNotification;

using System.Drawing;

namespace SoundAdjusterApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            NotifyIcon.Icon = SystemIcons.Application;
        }

        protected override void OnStateChanged( EventArgs e )
        {
            if ( WindowState == WindowState.Minimized )
            {
                this.Hide();
            }

            base.OnStateChanged( e );
        }

        private void TrayLeftMouseDownHandler(object sender, RoutedEventArgs e)
        {
            this.Show();
            this.WindowState = WindowState.Normal;
        }
    }
}
