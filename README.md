# EventOS Nano源码说明
-------
本代码命名为EventOS Nano，作为我未来的开源项目EventOS在小资源单片机（Cortex-M0/16/8位单片机）的精简版本。

### 由来
这个代码，原本是我在之前工作中做验证工作使用的，也曾在我的一个业余的AGV控制器项目上使用过。这个代码是对QP状态机框架的精简实现，包含其大部分功能，只有一个源文件和头文件，便于嵌入任何工程。不仅如此，里面还实现了完整的行为树框架，并与状态机框架融为一体，后续考虑删除，但感兴趣的可以看看。在消费类机器人领域里，会经常使用。在目前的工作中，公司采购了正版的QP，这个代码在工作中也没有了用武之地，索性开源，做点贡献。

### 代码结构
里面包含了一个基于Linux的CMake工程，一个基于LPC4088的MDK工程。代码经过了充分的单元测试，且在若干个项目里进行了使用，可靠性久经考验，是有保证的。核心代码分为以下几个部分：
+ **meowc/meow.c**
状态机框架与行为树框架的实现
+ **meowc/meow.h**
头文件
+ **meowc/meow_config.h**
对EventOS Nano进行配置与裁剪

其他代码的说明：
+ **main_meow.c**
测试工程的main函数，启动一个LED闪烁的状态机。
+ **main_unittest.c**
单元测试的main文件。
+ **meowc/port_linux.c**
在Linux平台上的接口实现。
+ **meowc/port_mcu.c**
在ARM Cortex-M4上接口实现。
+ **meowc/port_unittest.c**
单元测试所对应的接口实现。
+ **meowc/unittest_betree.c**
对框架中行为树部分的单元测试
+ **meowc/unittest_meow.c**
对框架中状态机部分的单元测试
+ **meowc/evt_def.c**
事件主题（枚举）的声明。
+ **mdebug**
曾经使用过的一个日志库，版本较老，当时实现的比较啰嗦，已经迭代了几个版本，新版本感觉还不错。日后，会以elog的名字，以独立组件的形式进行开源。

### EventOS Nano项目未来的开发路线
代码原来的名字叫Meow，是我老婆取的，因为她喜欢小猫的叫声。但这个名字多少有些不太严肃，所以改了。末来这个项目我会一直完善下去，作为一个私有项目，这个源码让我受益良多，但作为一个开源项目，目前已经完成的工作还远远不足。就我识别到的，将来需要完善的工你列举如下：
1. 良好的注释和文档
1. 将所有的变量和API以EventOS风格进行重构
1. 【完成】将构建工具修改为SCons
1. 增加内存管理功能，以支持任意长度的事件参数，修改掉原来的事件机制，优化RAM占用。
1. 将mdebug库删除，只保留断言接口，如果有日志必要，更换为elog库。（elog库，原名mlog，是我过去一直使用的日志库，整理后也会开源）
1. 将单元测试代码按照Unity框架进行重构
1. 对FSM模式的支持，以适用于最小资源的单片机ROM
1. 对HSM模式和发布-订阅模式的裁剪，以适用于最小资源的单片机ROM，优化RAM的占用
1. 对事件带参功能的裁剪，优化ROM的占用
1. 删除其中的行为树框架
1. 增加Doxygen风格的注释，并生成文档。
1. 对EventOS的eBridge（事件桥接）功能
1. 对ARM Cortex-M0 M3 M4 M7等单片机上的移植，增加对最常见型号单片机的支持，如STM32F103等。
1. 对常见的IDE的支持
1. 对常见的RTOS的支持
1. 增加EventOS抢占式内核
1. 增加线程模式
1. 增加对RISC-V内核的支持
1. 网友的反馈与要求

### 联系方式
邮箱：event-os@outlook.com

除了邮箱之外，也可以加微信联系我，请注明**技术讨论**等相关字样。

![avatar](/documentation/figures/wechat.jpg)