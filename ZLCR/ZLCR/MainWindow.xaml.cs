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

namespace ZLCR
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : System.Windows.Window
    {
        SerialPort ttyS0 = new System.IO.Ports.SerialPort();
        bool IsPortOpen = false;
        string PortName = null;
        byte[] buf = new byte[4000];
        double y1_io, y1_qo, y2_io, y2_qo;
        double y1_ioo, y1_qoo, y2_ioo, y2_qoo;
        double[] zlcr_Mag = new double[10];
        double[] zlcr_Phase = new double[10];

        MathNet.Filtering.FIR.OnlineFirFilter fir1_i = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir1_q = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir2_i = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir2_q = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));

        MathNet.Filtering.FIR.OnlineFirFilter fir1_io = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir1_qo = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir2_io = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));
        MathNet.Filtering.FIR.OnlineFirFilter fir2_qo = new MathNet.Filtering.FIR.OnlineFirFilter(MathNet.Filtering.FIR.FirCoefficients.LowPass(125000, 125, 200));

        public MainWindow()
        {
            InitializeComponent();

            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                _portChoose.Items.Add(port);
            }
            if (_portChoose.Items.Count != 0)
                _portChoose.SelectedIndex = 0;

            Thread tty_thread = new Thread(ttyS0_thread);
            tty_thread.IsBackground = true;
            tty_thread.Start();
        }

        private void ttyS0_thread()
        {
            ttyS0.PortName = "COM1";
            //ttyS0.BaudRate = 9600;
            //ttyS0.DataBits = 8;
            //ttyS0.StopBits = StopBits.One;
            //ttyS0.NewLine = "\r\n";
            ttyS0.ReadTimeout = 500;
            ttyS0.ReadBufferSize = 100000;
            ttyS0.WriteBufferSize = 100000;


            for (; ; )
            {
                Thread.Sleep(2);

                try
                {
                    this.Dispatcher.Invoke(new Action(() =>
                    {
                        PortName = _portChoose.SelectedItem.ToString();
                        text.Text = zlcr_Mag[4].ToString("f2") + "\r\n" + zlcr_Phase[4].ToString("f2");
                    }));

                    if (IsPortOpen)
                    {
                        if (ttyS0.IsOpen)
                        {
                            int count = ttyS0.BytesToRead;
                            if (count != 0)
                            {
                                //ttyS0.ReadExisting();
                                //System.Diagnostics.Debug.WriteLine(count);
                                if (count > 4000)
                                {
                                    ttyS0.Read(buf, 0, 4000);

                                    for(int i=0;i<500;i++)
                                    {
                                        //System.Diagnostics.Debug.WriteLine(fir1_i.ProcessSample(BitConverter.ToInt16(buf, 2*i)));
                                        //System.Diagnostics.Debug.WriteLine(BitConverter.ToInt16(buf, 2 * i));
                                        y2_io = fir1_i.ProcessSample(BitConverter.ToInt16(buf, (i << 1)));
                                        y2_qo = fir1_q.ProcessSample(BitConverter.ToInt16(buf, (i << 1) + 1000));
                                        y1_io = fir2_i.ProcessSample(BitConverter.ToInt16(buf, (i << 1) + 2000));
                                        y1_qo = fir2_q.ProcessSample(BitConverter.ToInt16(buf, (i << 1) + 3000));
                                    }

                                    //y1_ioo = fir1_io.ProcessSample(y1_io);
                                    //y1_qoo = fir1_io.ProcessSample(y1_qo);
                                    //y2_ioo = fir1_io.ProcessSample(y2_io);
                                    //y2_qoo = fir1_io.ProcessSample(y2_qo);

                                    //System.Diagnostics.Debug.WriteLine("{0},{1},{2},{3}",y1_ioo, y1_qoo, y2_ioo, y2_qoo);
                                    zlcr_Mag[4] = 1000 * Math.Sqrt(y1_io * y1_io + y1_qo * y1_qo) / Math.Sqrt(y2_io * y2_io + y2_qo * y2_qo);
                                    zlcr_Phase[4] = 360 / 2 / Math.PI * Math.Acos((y1_io * y2_io + y1_qo * y2_qo) / (Math.Sqrt(y1_io * y1_io + y1_qo * y1_qo) * Math.Sqrt(y2_io * y2_io + y2_qo * y2_qo)));

                                    //System.Diagnostics.Debug.WriteLine("{0}\t\t{1}", zlcr_Mag[4], zlcr_Phase[4]);
                                }
                            }
                        }
                        else
                        {
                            ttyS0.PortName = PortName;
                            ttyS0.Open();
                        }
                    }
                    else
                    {
                        if (ttyS0.IsOpen)
                        {
                            ttyS0.Close();
                            Thread.Sleep(50);
                        }
                    }
                }
                catch (Exception ex)
                {
                    IsPortOpen = false;
                    this.Dispatcher.Invoke(new Action(() => { _portOpen.Content = "连接"; }));
                    MessageBox.Show(ex.Message);
                }
            }
        }


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (ttyS0.IsOpen)
            {
                IsPortOpen = false;
                _portOpen.Content = "连接";
            }
            else
            {
                IsPortOpen = true;
                _portOpen.Content = "关闭";
            }
        }
    }
}
