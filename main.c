#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// ��������Ŷ��� - ����ʵ�ʵ�·�޸�
#define SEG_PORT PORTB
#define SEG_DDR  DDRB
#define DIG_PORT PORTA
#define DIG_DDR  DDRA

// ��������ܶ����(0-9)
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

volatile uint8_t seconds = 30; // ��ʼ����ʱʱ��30��
volatile uint8_t digit_pos = 0; // ��ǰ��ʾλ
volatile uint8_t timer_cnt = 0; // ��ʱ��������

// ��ʾһ��������ָ��λ��
void display_digit(uint8_t pos, uint8_t num) {
	// �ȹر�����λѡ
	DIG_PORT |= (1 << PA0) | (1 << PA1);
	
	// �������
	SEG_PORT = seg_table[num];
	
	// ѡ���Ӧ��λ
	if(pos == 0) {
		DIG_PORT &= ~(1 << PA0); // ����ʮλ
		} else {
		DIG_PORT &= ~(1 << PA1); // ������λ
	}
}

// ��ʱ��1�Ƚ�ƥ���ж� - ����1�붨ʱ
ISR(TIMER1_COMPA_vect) {
	timer_cnt++;
	if(timer_cnt >= 100) { // 10ms * 100 = 1��
		timer_cnt = 0;
		if(seconds > 0) {
			seconds--;
		}
	}
}

// ��ʱ��0����ж� - ��������ܶ�̬ɨ��
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
	// ��ʱ��0 - ��������ܶ�̬ɨ��(Լ1ms�ж�һ��)
	TCCR0 = (1 << CS01); // ��Ƶϵ��8
	TIMSK = (1 << TOIE0); // ʹ������ж�
	TCNT0 = 0;
}

void init_timer1() {
	// ��ʱ��1 - ����1�붨ʱ
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTCģʽ, ��Ƶϵ��64
	OCR1A = 156; // 10ms�ж�һ�� (16MHz/64/156 �� 10ms)
	TIMSK |= (1 << OCIE1A); // ʹ�ܱȽ�ƥ���ж�
}

void init_io() {
	// ��������ܶ�ѡ�˿�Ϊ���
	SEG_DDR = 0xFF;
	// ����λѡ�˿�Ϊ���
	DIG_DDR |= (1 << PA0) | (1 << PA1);
	// ��ʼ�ر������
	DIG_PORT |= (1 << PA0) | (1 << PA1);
}

int main(void) {
	init_io();
	init_timer0();
	init_timer1();
	
	sei(); // ����ȫ���ж�
	
	while(1) {
		// ��ѭ�����������������
		// ���簴����������õ���ʱ��
	}
	
	return 0;
}
