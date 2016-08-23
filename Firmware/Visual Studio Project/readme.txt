========================================================================
    生成文件项目：Firmware 项目概述
========================================================================

本文件概要介绍组成 Firmware 项目的每个文件的内容。

Firmware.sln
    这是Firmware项目的解决方案文件

Firmware.vcxproj
    这是Firmware项目的主项目文件
    其中包含了这个项目中的各个Target，
    以及Include Path、所有源文件的路径、编译命令。

Firmware.vcxproj.filters
    这是Firmware项目的项目筛选器文件。
    它包含了这个项目中的所有源文件分组及源文件的路径。

Firmware.vcxproj.user
    这是Firmware项目的 用户文件，
    它包含了这个项目中的各个Target的 Debug命令。

以上文件由MDK Project  To Visual Studio Project 工具读取 Keil uVision4
的项目文件：Firmware.uvproj 中的设定，按照Visual Studio 2010
中VC++ “生成文件项目” 的模板文件来生成的，如有疑问，请看MSDN~

2012-6-18
Ka_Chen
/////////////////////////////////////////////////////////////////////////////
