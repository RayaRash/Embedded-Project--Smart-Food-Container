
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)
#define BSIZE 128
#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <stdint.h>

volatile unsigned rxData;
volatile unsigned rxHumData;
volatile unsigned PreGasData =4;
volatile unsigned GasData = 5;
volatile unsigned PreHumRXData;
volatile unsigned ADC_value;
volatile unsigned Mode;
int i =0;
#include "lcd.h"
#include "lcd.c"
void action(void);

void initUSART(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8);
	UBRR0L = (unsigned char)(UBRR_VALUE);
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
	sei();
}

unsigned char receiveUSART(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
void initADC(void)
{
	ADCSRA |= (1<<ADEN);
	ADCSRA |= 0B00000111;
	ADCSRA |= (1<<ADSC);
}
void ModeCheck(void)
{
	if (Mode == 1)
	{
		LCDWriteStringXY(0,1,"Mode: Fruits");
	}
	else
	{
		LCDWriteStringXY(0,1,"Mode: Vegetables");
	}
}
void Edible(void)
{
	LCDClear();
	ModeCheck();
	LCDWriteStringXY(0,2,"Food Status:");
	LCDWriteStringXY(1,3,"Edible                     ");
	PreHumRXData = rxHumData;
	PreGasData = GasData;
	PORTB |= 0B00010000;
	PORTB &= 0B00011111;
}
void Spoiled(void)
{
	LCDClear();
	ModeCheck();
	LCDWriteStringXY(0,2,"Food Status:");
	LCDWriteStringXY(0,3,"Spoiled                      ");
	PreHumRXData = rxHumData;
	PreGasData = GasData;
	PORTB |= 0B00100010;
	PORTB &= 0B00101111;
}

int main(void)
{
	EICRA |= 0x03;
	EIMSK |= 0x01;
	sei();
	
	initUSART();
	DDRB |= 0B00000010; // PORT B PIN 1 as output
	DDRB |= 0B00000100; // PORT B PIN 2 as output
	initADC();
	LCDInit(LS_BLINK);
	
	while(1)
	{
		if (i==4)
		{
			LCDWriteStringXY(13,0,"g");
			LCDFirstLine();
			LCDCur();
			LCDWriteStringXY(0,0,"Weight = ");
			//LCDLeftLine();
			i = 0;
		}
		/*int digit1s = 0;
		int digit10s = 0;
		int digit100s = 0;
		int digit1000s = 0;
		*/
		ADMUX = (ADMUX & 0xF0) | 0;
		ADMUX &= 0B00111111;
		ADCSRA |= (1<<ADSC);
		
		while (ADCSRA & (1<<ADSC));
		ADC_value = ADC;
		//LCDWriteInt(ADC_value,-1);
		//_delay_ms(1000);
		if (PIND & 0B10000000)
		{
			Mode = 1;
			//LCDWriteStringXY(0,1,"Mode: Fruits");
		}
		else
		{
			Mode = 2;
			//LCDWriteStringXY(0,1,"Mode: Vegetables");
		}
		if (Mode == 1)
		{
			//Fruit
			if(ADC_value>100)
			{
				GasData = 1;
			}
			else
			{
				GasData = 0;
			}
		}
		else
		{
			//veg
			if(ADC_value>120)
			{
				GasData = 3;
			}
			else
			{
				GasData = 2;
			}
		}
		
		//_delay_ms(50);
		//LCDWriteInt(rxData,-1);
		if (PreHumRXData != rxHumData || PreGasData != GasData)
		{
			if(GasData == 1 || GasData == 3)
			{
				Spoiled();
			}
			else
			{
				if (rxHumData == 1)
				{
					Spoiled();
				}
				else
				{
					Edible();
				}
			}
		}
		
		
		/*digit1s = ADC_value%10;
		digit10s = (ADC_value%100)/10;
		digit100s = (ADC_value%1000)/100;
		digit1000s = (ADC_value%10000)/1000;
		LCDHexDumpXY(0,3,digit1000s);
		LCDHexDumpXY(3,3,digit100s);
		LCDHexDumpXY(6,3,digit10s);
		LCDHexDumpXY(9,3,digit1s);*/
	}
	return 0;
}

void action(void)
{
	rxData = receiveUSART();
	//LCDWriteInt(rxData,-1);
	if (rxData < 65)
	{
		LCDWriteInt(rxData,1);
		i++;
		//LCDWriteStringXY(4,1,"gfsghdfhg");	
	}
	else
	{
		if ((rxData == 65))
		{
			rxHumData = 1;
			//LCDHexDumpXY(1,2,rxData);
		}
		else
		{
			rxHumData = 0;
		}	
	}	
}
ISR (USART_RX_vect)
{
	action();
}