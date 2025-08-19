#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// 数码管引脚定义 - 根据实际电路连接修改
#define SEG_PORT PORTD
#define SEG_DDR  DDRD
#define DIG_PORT PORTB
#define DIG_DDR  DDRB

// 位选引脚定义
#define DIGIT_TEN  PB0  // 十位
#define DIGIT_ONE  PB1  // 个位

// 共阳数码管段码表(0-9)
const uint8_t seg_table[] = {
    0xC0, // 0 - a,b,c,d,e,f
    0xF9, // 1 - b,c
    0xA4, // 2 - a,b,d,e,g
    0xB0, // 3 - a,b,c,d,g
    0x99, // 4 - b,c,f,g
    0x92, // 5 - a,c,d,f,g
    0x82, // 6 - a,c,d,e,f,g
    0xF8, // 7 - a,b,c
    0x80, // 8 - a,b,c,d,e,f,g
    0x90  // 9 - a,b,c,d,f,g
};

volatile uint8_t seconds = 30;  // 初始倒计时时间30秒
volatile uint8_t digit_pos = 0; // 当前显示位
volatile uint16_t ms_counter = 0; // 毫秒计数器

// 显示一个数字在指定位置
void display_digit(uint8_t pos, uint8_t num) {
    // 先关闭所有位选
    DIG_PORT |= (1 << DIGIT_TEN) | (1 << DIGIT_ONE);
    
    // 输出段码
    SEG_PORT = seg_table[num];
    
    // 选择对应的位
    if(pos == 0) {
        DIG_PORT &= ~(1 << DIGIT_TEN); // 开启十位
    } else {
        DIG_PORT &= ~(1 << DIGIT_ONE); // 开启个位
    }
}

// 定时器1比较匹配A中断 - 用于1ms定时和数码管扫描
ISR(TIMER1_COMPA_vect) {
    // 数码管动态扫描（约2ms切换一次位）
    static uint8_t scan_counter = 0;
    scan_counter++;
    
    if(scan_counter >= 2) { // 每2ms切换一次显示位
        scan_counter = 0;
        uint8_t ten = seconds / 10;
        uint8_t one = seconds % 10;
        
        if(digit_pos == 0) {
            display_digit(0, ten);
            digit_pos = 1;
        } else {
            display_digit(1, one);
            digit_pos = 0;
        }
    }
    
    // 1秒定时
    ms_counter++;
    if(ms_counter >= 1000) { // 1000ms = 1秒
        ms_counter = 0;
        if(seconds > 0) {
            seconds--;
        }
    }
}

void init_timer1() {
    // 定时器1 - CTC模式，用于1ms定时
    TCCR1A = 0; // 普通模式
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC模式, 分频系数8
    OCR1A = 1999; // 1ms中断 (16MHz/8/2000 = 1ms)
    TIMSK1 = (1 << OCIE1A); // 使能比较匹配A中断
}

void init_io() {
    // 设置数码管段选端口(PD0-PD7)为输出
    SEG_DDR = 0xFF;
    // 设置位选端口(PB0,PB1)为输出
    DIG_DDR |= (1 << DIGIT_TEN) | (1 << DIGIT_ONE);
    // 初始关闭数码管
    DIG_PORT |= (1 << DIGIT_TEN) | (1 << DIGIT_ONE);
    // 初始显示00
    SEG_PORT = seg_table[0];
}

void init_timer0() {
    // 定时器0 - 用于简单的延时，8位定时器
    TCCR0A = (1 << WGM01); // CTC模式
    TCCR0B = (1 << CS01) | (1 << CS00); // 分频系数64
    OCR0A = 250; // 约1ms中断
}

int main(void) {
    // 初始化
    init_io();
    init_timer1();
    
    sei(); // 开启全局中断
    
    // 初始显示
    display_digit(0, seconds / 10);
    _delay_ms(2);
    display_digit(1, seconds % 10);
    _delay_ms(2);
    
    while(1) {
        // 主循环可以添加其他功能
        // 例如：按键检测、重置倒计时等
        
        // 简单的延时，避免主循环空转
        _delay_ms(100);
    }
    
    return 0;
}
