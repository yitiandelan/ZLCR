## 概述

基于低成本硬件和 DLIA 算法实现 LCR 表，主要特性是：

* 电路结构简单，低成本；
* 激励信号幅度：0.25 Vrms；
* 激励信号频率：[1e0, 1e5] Hz，步进：0.01 Hz;
* 待测阻抗范围：[1e-3, 1e7] Ohm；

核心类似 SDR 中的数字下变频（数字正交解调）算法，准确的定义是 DLIA（Digital Lock-in amplifier，数字锁定放大器）；利用数字信号处理算法代替部分硬件电路，降低硬件复杂程度。

与主流方案的区别：

* PSD 电路，测量范围更大，当信号转换到数字域后，就无需再考虑噪声和失调；
* FFT、LMS算法，分辨率更高，调整滤波器带宽，可以得到更高信噪比；

测试截图：

* [测试某变压器](doc/res/app_csharp_4e-3h.gif)
* [扫频测试](doc/res/app_python_scan_dut1.png)
* [测试建立时间](doc/res/app_python_setup_1e-8f_1e3hz.png)
* [更多图片](doc/res)

## 硬件相关

板载调试器为 J-Link OB-STM32F072-CortexM / CMSIS-DAP 兼容设计，DFU 自举；

## 软件相关

## 扩展部分

运放构成的自平衡电桥不适合工作在 100 kHz 以上频率，设计了数字平衡电桥结构，由两个 DDS 产生激励，检测不平衡电压/电流，控制激励信号幅度/相位，使桥路趋近平衡（减小误差）；同时采样 V(DUT) & I(DUT)，计算复阻抗；通过欠采样解决 ADC 采样率不够和计算量增大的问题，解决方案是欠采样（中频采样），设计了简单的采样保持器 (暂未验证)；设计通过 PWM（AF OD 模式）调整 I(FSADJSET) AD9834 输出幅度，通过抖动实现 16 bit 相位控制；

最后，发个还未验证的版本：[ZLCR Plus Sch](hardware/zlcr_plus_sch_rev.a.pdf)

## 参考

[Keysight Technologies Impedance Measurement Handbook](http://literature.cdn.keysight.com/litweb/pdf/5950-3000.pdf)

[抛砖引玉 基于DSP的LCR表试制 供大家参考](http://www.amobbs.com/thread-5590156-1-1.html)

[MT-002: 奈奎斯特准则对数据采样系统设计有何意义](http://www.analog.com/media/cn/training-seminars/tutorials/MT-002_cn.pdf)

[MS-2698：使用同步检测进行精密低电平测量](http://www.analog.com/media/cn/technical-documentation/technical-articles/Use-Synchronous-Detection-to-Make-Precision-Low-Level-Measurements-MS-2698_cn.pdf)

[基于DLIA的交流阻抗谱测量系统关键技术研究](http://cdmd.cnki.com.cn/Article/CDMD-10487-1012361681.htm)