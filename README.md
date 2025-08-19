# AVR-
这是一个基于AVR单片机(如ATmega16/32)控制两位共阳数码管进行倒计时的C语言程序。程序使用定时器中断实现1秒定时，并通过动态扫描方式显示倒计时数值。
电路连接说明（ATmega328P）
段选连接：

PD0 → 数码管a段

PD1 → 数码管b段

PD2 → 数码管c段

PD3 → 数码管d段

PD4 → 数码管e段

PD5 → 数码管f段

PD6 → 数码管g段

PD7 → 数码管dp段

位选连接：

PB0 → 十位数码管公共端

PB1 → 个位数码管公共端

电源：

数码管需要串联限流电阻(220Ω-1kΩ)

使用5V电源供电

编译和烧录
使用AVR-GCC编译：

bash
avr-gcc -mmcu=atmega328p -Os -o countdown.elf countdown.c
avr-objcopy -O ihex countdown.elf countdown.hex
使用AVRDude烧录：

bash
avrdude -p atmega328p -c arduino -P COMx -b 115200 -U flash:w:countdown.hex
功能扩展建议
添加按键控制：

c
// 添加重置按钮
if (!(PINB & (1 << PB2))) { // 假设PB2接按钮
    seconds = 30; // 重置倒计时
    ms_counter = 0;
}
添加蜂鸣器报警：

c
// 倒计时结束时触发蜂鸣器
if (seconds == 0) {
    // 控制蜂鸣器引脚
}
添加暂停/继续功能：

c
// 使用另一个按钮控制暂停
注意事项
确保使用16MHz晶振或设置正确的熔丝位

数码管为共阳类型，如果使用共阴数码管需要修改段码表

实际连接时务必添加适当的限流电阻

如果使用不同的引脚，需要相应修改端口定义

这个程序应该能够在ATmega328P上正常运行，实现两位数码管的倒计时显示功能。
