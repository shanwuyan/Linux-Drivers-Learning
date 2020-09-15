# Linux-Drivers-Learning
To record the code written by me during the learning process/记录Linux驱动学习过程中所写的代码

如何编写一个简单的Linux驱动（一）——驱动的基本框架：<br/>
    &emsp;&emsp;包括驱动入口函数、出口函数、两个函数的注册、开源协议。实现的功能是加载和卸载驱动时打印相应的字符串。
<br/><br/>
如何编写一个简单的Linux驱动（二）——设备操作集file_operations：<br/>
    &emsp;&emsp;结构体file_operations为应用程序提供了操作设备的接口，包括打开设备、关闭设备、读设备、写设备等操作。
<br/><br/>
如何编写一个简单的Linux驱动（三）——完善设备驱动<br/>
    &emsp;&emsp;实现了自动创建设备节点、由系统分配设备号、和用户程序交换数据。
<br/><br/>
Linux驱动之设备树的基础知识<br/>
    &emsp;&emsp;这一部分没有代码。
<br/><br/>
Linux驱动之GPIO子系统和pinctrl子系统<br/>
    &emsp;&emsp;这部分通过设备树pinctrl子系统以及GPIO子系统，实现了用户程序对GPIO的控制。用户程序可以控制两个LED灯的亮和灭。
 