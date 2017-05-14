`note:` 
移植ucos-II，目标开发板：mini2440(S3C2440)
扩展uCOS-II的源码，添加一些源码中没有的模块
如：中断处理 shell程序 驱动

###开发环境
1. 系统：redhat6
2. 编译器：arm-linux-gcc （`note`推荐版本为4.3.2，高版本可能会出现很难排查的编译错误）
3. norflash操作系统：supervivi（`note`可以使用uboot，并通过以太网下载，但是小系统不推荐）
4. 下载工具：usb串口线下载，切记安装dnw

###shell支持的指令
1. ledon 打开led
2. ledoff 关闭led
3. beepon 打开蜂鸣器
4. beepoff 关闭蜂鸣器
5. sysinfo 系统信息
6. help 帮助

###鸣谢
整个系统自己造轮子的并不多，有太多的开源代码可供参考，正因为站在巨人的肩膀上，我们才看的更远，感谢这个开源的世界，Thanks for this open source world!!

###联系方式
邮箱：fjlxq1994@126.com
有兴趣研究操作系统的道友可有下载源码，并随意更改，写的好，默默点赞，写的不好，不要骂街！

（PS：一只即将毕业的大四狗 2017-5-14 22:48:37）