#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// 数码管引脚定义 - 根据实际电路修改
#define SEG_PORT PORTB
#define SEG_DDR  DDRB
#define DIG_PORT PORTA
#define DIG_DDR  DDRA

// 共阳数码管段码表(0-9)
const uint8_t seg_table[] = {
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x90  // 9
};

volatile uint8_t seconds = 30; // 初始倒计时时间30秒
volatile uint8_t digit_pos = 0; // 当前显示位
volatile uint8_t timer_cnt = 0; // 定时器计数器

// 显示一个数字在指定位置
void display_digit(uint8_t pos, uint8_t num) {
	// 先关闭所有位选
	DIG_PORT |= (1 << PA0) | (1 << PA1);
	
	// 输出段码
	SEG_PORT = seg_table[num];
	
	// 选择对应的位
	if(pos == 0) {
		DIG_PORT &= ~(1 << PA0); // 开启十位
		} else {
		DIG_PORT &= ~(1 << PA1); // 开启个位
	}
}

// 定时器1比较匹配中断 - 用于1秒定时
ISR(TIMER1_COMPA_vect) {
	timer_cnt++;
	if(timer_cnt >= 100) { // 10ms * 100 = 1秒
		timer_cnt = 0;
		if(seconds > 0) {
			seconds--;
		}
	}
}

// 定时器0溢出中断 - 用于数码管动态扫描
ISR(TIMER0_OVF_vect) {
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

void init_timer0() {
	// 定时器0 - 用于数码管动态扫描(约1ms中断一次)
	TCCR0 = (1 << CS01); // 分频系数8
	TIMSK = (1 << TOIE0); // 使能溢出中断
	TCNT0 = 0;
}

void init_timer1() {
	// 定时器1 - 用于1秒定时
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC模式, 分频系数64
	OCR1A = 156; // 10ms中断一次 (16MHz/64/156 ≈ 10ms)
	TIMSK |= (1 << OCIE1A); // 使能比较匹配中断
}

void init_io() {
	// 设置数码管段选端口为输出
	SEG_DDR = 0xFF;
	// 设置位选端口为输出
	DIG_DDR |= (1 << PA0) | (1 << PA1);
	// 初始关闭数码管
	DIG_PORT |= (1 << PA0) | (1 << PA1);
}

int main(void) {
	init_io();
	init_timer0();
	init_timer1();
	
	sei(); // 开启全局中断
	
	while(1) {
		// 主循环可以添加其他功能
		// 例如按键检测来重置倒计时等
	}
	
	return 0;
}
