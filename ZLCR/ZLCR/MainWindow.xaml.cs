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

using System.IO.Ports;
using System.Threading;

using MathNet.Numerics;
using MathNet.Filtering;
using Newtonsoft.Json;

namespace ZLCR
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : System.Windows.Window
    {
        SerialPort ttyS0 = new System.IO.Ports.SerialPort();
        string str1;
        float freq, mag, phase = 0;

        //MathNet.Filtering.FIR.OnlineFirFilter fir1_i = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));

        public MainWindow()
        {
            InitializeComponent();

            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            foreach (string port in ports)
                _portChoose.Items.Add(port);
            if (_portChoose.Items.Count != 0)
                _portChoose.SelectedIndex = 0;

            Thread tty_thread = new Thread(ttyS0_thread);
            tty_thread.IsBackground = true;
            tty_thread.Start();
        }

        private void ttyS0_thread()
        {
            ttyS0.PortName = "COM1";
            ttyS0.BaudRate = 115200;

            for (; ; )
            {
                Thread.Sleep(5);

                try
                {
                    if (ttyS0.IsOpen)
                    {
                        str1 = ttyS0.ReadLine();
                        var js = JsonConvert.DeserializeObject<Dictionary<string, float>>(str1);

                        js.TryGetValue("FREQ", out freq);
                        js.TryGetValue("MAG", out mag);
                        js.TryGetValue("PHASE", out phase);

                        this.Dispatcher.Invoke(new Action(() =>
                        {
                            if (mag > 10000)
                                mag = 10000;
                            text.Text = string.Format("{0:N2} Hz\n{1:N4} ohm\n{2:N4} °", freq, mag*1000, phase*180/Math.PI);
                        }));
                    }
                }
                catch (Exception ex)
                {
                    //MessageBox.Show(ex.Message);
                }
            }
        }

        private void textBox_KeyUp(object sender, KeyEventArgs e)
        {
            if(e.Key == System.Windows.Input.Key.Enter)
                if (ttyS0.IsOpen)
                    ttyS0.WriteLine(textBox.Text);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (ttyS0.IsOpen)
            {
                _portOpen.Content = "连接";
                ttyS0.Close();
            }
            else
            {
                _portOpen.Content = "关闭";
                ttyS0.PortName = _portChoose.SelectedItem.ToString();
                ttyS0.Open();
                //ttyS0.WriteLine("reboot");
            }
        }

        private void button_Click_1(object sender, RoutedEventArgs e)
        {
            if (ttyS0.IsOpen)
                ttyS0.WriteLine(textBox.Text);
        }
    }
}
