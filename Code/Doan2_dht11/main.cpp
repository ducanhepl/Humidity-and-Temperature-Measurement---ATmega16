/*
 * Doan2_DHT11.cpp
 *
 * Created: 3/14/2023 8:19:38 AM
 * Author : ADMIN
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/sfr_defs.h>

#define DHT11_PIN 0

#define DB DDRB
#define DC DDRC
#define DD DDRD
#define PB PORTB
#define PC PORTC
#define PD PORTD
#define	RS PIND6
#define	RW PIND5
#define	RE PIND7

uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void Request()				/* VDK gui xung yeu cau */
{
	DB |= (1<<DHT11_PIN);
	PB &= ~(1<<DHT11_PIN);	/* cho chan PB o muc logic thap */
	_delay_ms(20);			/* cho xung thap dai 20ms */
	PB |= (1<<DHT11_PIN);	/* keo chan PB len muc logic cao*/
}

void Response()				/* nhan phan hoi tu DHT11 */
{
	DB &= ~(1<<DHT11_PIN);
	while(PINB & (1<<DHT11_PIN));        /* xung cao */
	while((PINB & (1<<DHT11_PIN))==0);   /* xung thap dai 54us */
	while(PINB & (1<<DHT11_PIN));        /* xung cao dai 80us */
}

uint8_t Receive_data()			/* ham nhan du lieu tu DHT11 */
{
	for (int q=0; q<8; q++)
	{
		while((PINB & (1<<DHT11_PIN)) == 0);  /* cho doan tin hieu xung thap */
		_delay_us(30);
		if(PINB & (1<<DHT11_PIN))/* neu xung cao dai hon 30us */
		{
			c = (c<<1)|(0x01);	/* thi gia tri logic tra ve co gia tri 1 */
		}
		else			/* neu khong gia tri logic tra ve la 0 */
		{
			c = (c<<1);
		}
		while(PINB & (1<<DHT11_PIN));
	}
	return c;
}

void setup_lcd(unsigned char x) {
	PC = x;
	PD &=~(1<<RS);  // RS = 0 chon thanh ghi lenh IR
	PD &=~(1<<RW);  // RW = 0 hoat dong o che do ghi
	PD |=(1<<RE);  // RE = 1 bat dau khung truyen
	_delay_us(1);
	PD &=~(1<<RE);  // RE = 0 ket thuc khung truyen
	_delay_ms(3);
}

void display_char(unsigned char x) {
	PC = x;
	PD |=(1<<RS);  // RS = 1 chon thanh ghi DR
	PD &=~(1<<RW);  // RW = 0 hoat dong o che do ghi
	PD |=(1<<RE);  // RE = 1 bat dau khung truyen
	_delay_us(1);
	PD &=~(1<<RE);  // RE = 0 ket thuc khung truyen
	_delay_ms(2);
}

void khoitao(void) {
	DC = 0XFF;  //output du lieu
	DD = 0XFF;
	_delay_ms(20);
	setup_lcd(0x38);  // truyen 8 bit va su dung 2 hang
	setup_lcd(0x0C);  //bat man hinh va tat con tro
	setup_lcd(0x06);  // con tro dich phai
	setup_lcd(0x01);  // xoa man hinh
	setup_lcd(0x80);  // dua con tro ve vi tri ban dau
}

void display_string(char *str) {  // ham hien thi chuoi ky tu
	for (int i = 0; str[i] != 0; i++) {
		display_char(str[i]);
	}
}

void display_stringxy(char row, char pos, char *str) {
	if (row == 0 && pos < 16) {
		setup_lcd((pos & 0x0F) | 0x80);
	}
	else if (row == 1 && pos < 16) {
		setup_lcd((pos & 0x0F) | 0xC0);
	}
	display_string(str);
}

void clear_lcd() {
	setup_lcd(0x01);
	setup_lcd(0x80);
}


int main(void)
{
    khoitao();
    char nhietdo[5];
    char doam[5];
    
    _delay_ms(100);
	
    while (1) 
    {
		Request();		/* gui xung yeu cau */
		Response();		/* nhan phan hoi */
		I_RH=Receive_data();	/* luu 8 bit dau tien vao I_RH */	
		D_RH=Receive_data();	/* luu 8 bit tiep theo vao D_RH */
		I_Temp=Receive_data();	/* luu 8 bit tiep theo vao I_Temp */
		D_Temp=Receive_data();	/* luu 8 bit tiep theo vao D_Temp */
		CheckSum=Receive_data();/* luu 8 bit tiep theo vao CheckSum */
		
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			display_stringxy(0, 0, "ERROR!!");
		}
		else
		{
			display_stringxy(0, 0, "nhiet do: ");
			sprintf(nhietdo,"%d\337C", I_Temp);
			display_stringxy(0, 10, nhietdo);
			
			display_stringxy(1, 0, "do am: ");
			sprintf(doam,"%d ", I_RH);
			display_stringxy(1, 7, doam);
			display_stringxy(1, 9, "%");		
		}
		
		_delay_ms(10);
    }
}