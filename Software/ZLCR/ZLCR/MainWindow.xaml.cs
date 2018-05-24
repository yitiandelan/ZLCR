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
using System.Numerics;

namespace ZLCR
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : System.Windows.Window
    {
        SerialPort ttyS0 = new System.IO.Ports.SerialPort();
        string str1;
        float freq, a, b, c, d;
        Complex Num1, Num2, z;

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
                        js.TryGetValue("a", out a);
                        js.TryGetValue("b", out b);
                        js.TryGetValue("c", out c);
                        js.TryGetValue("d", out d);

                        Num1 = new Complex(a , b );
                        Num2 = new Complex(c , d );

                        z = Complex.Divide(Num1, Num2) * (1000 );


                        //System.Diagnostics.Debug.WriteLine(z);

                        this.Dispatcher.Invoke(new Action(() =>
                        {
                            //text.Text = string.Format("{0:N2} Hz\n{1:N} {2:N}i\n{3:N} {4:N}i\n{5:N6} {6:N6}i", freq, a, b, c, d, z.Real, z.Imaginary);
                            //text.Text = string.Format("freq(Hz):  {0:000,000.00}\nlog|z|(ohm):  {1:0000000,0.00}\nphase(deg):  {2:000.0000}", freq, z.Magnitude, z.Phase * 180 / Math.PI);
                            text.Text = string.Format("{0:0,0.00}\n{1:0,0.00}\n{2:0.0000}\n", freq, z.Magnitude, z.Phase * 180 / Math.PI);
                            text.Text += string.Format("{0:0,0.00}\n", Math.Abs(z.Real));

                            if (z.Phase > 0.5)
                            {
                                text.Text += string.Format("{0:0,0.00}\n\n", 1.0 / 2.0 / Math.PI / freq / Math.Abs(z.Imaginary) * 1000000000000);
                                text.Text += string.Format("{0:0,0.000}", Math.Abs(z.Real) / Math.Abs(z.Imaginary));
                            }
                                
                            else if (z.Phase < -0.5)
                            {
                                text.Text += string.Format("\n{0:0,0.000}\n\n", Math.Abs(z.Imaginary) / 2.0 / Math.PI / freq * 1000000);
                                text.Text += string.Format("{0:0,0.000}", Math.Abs(z.Imaginary) / Math.Abs(z.Real));
                            }
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
