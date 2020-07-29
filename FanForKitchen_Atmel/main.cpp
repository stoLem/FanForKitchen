/*
 * FanForKitchen.cpp
 *
 * Created: 21.07.2020 17:29:03
 * Author : Константин
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

ISR (TIM0_COMPA_vect)
{
	TCNT0=0;

	if (PORTB & (1<<PB0))
	PORTB&=~(1<<PB0);
	else
	PORTB|=(1<<PB0);
}

int main(void)
{
	
	DDRB |= 0b00100000;

	//Включим ножку INT0 (PD2) на вход

	DDRD &= ~(0b00000100);

	//Подтянем резистор на ножке INT0 (PD2) к питанию

	  PORTD |= 0b00000100;
	DDRB |= (1<<DDB1);//установка 1-го бита в 1 порта DDRB (ножка PB1 или OC1A), порт опрделён на выход
	
    /* Replace with your application code */
    while (1) 
    {

    }
	
}

