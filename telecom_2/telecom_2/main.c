
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define FOSC 16000000// Clock Speed
#define BAUD 38400
#define MYUBRR (FOSC/16/BAUD-1)

uint16_t x_axis = 0;
uint16_t y_axis = 0;
uint16_t x_servo = 0;
uint16_t y_servo = 0;



void ADC_init(void)
{
	ADMUX |=(1<<REFS0);
	ADCSRB |=(1<<MUX5);
	ADCSRA |=(1<<ADEN) |(1<<ADPS2)|(1<<ADPS1) |(1<<ADPS0) ;
	
}
void USART_Init (unsigned int ubrr )
{
	UBRR0H = (unsigned char)(ubrr>>8);                 // set baud rate
	UBRR0L = (unsigned char)ubrr;                      // set baud rate
	UCSR0B = (1<<RXEN0)|(1<<TXEN0) ;                   // enable reciver and transmitter
	UCSR0C = (3<<UCSZ00);                              // set frame format: 8data, 1stop bit
	
}


int main(void)
{
	
	 ADC_init();
	 USART_Init (MYUBRR);
	 
	 
    while (1) 
    {
			ADMUX = 0b01000000;
			ADCSRA |=(1<<ADSC);
			while (( ADCSRA & (1<<ADSC)));
			x_axis = ( ADC/4);
			UDR0 = (uint8_t)x_axis;
			while ( !( UCSR0A & (1<<UDRE0)) );
		
			ADMUX = 0b01000001;
			ADCSRA |=(1<<ADSC);
			while (( ADCSRA & (1<<ADSC)));
			y_axis = ( ADC/4);
			UDR0 = (uint8_t)y_axis;
			while ( !( UCSR0A & (1<<UDRE0)) );
			
			ADMUX = 0b01000010;
			ADCSRA |=(1<<ADSC);
			while (( ADCSRA & (1<<ADSC)));
			x_servo = ( ADC/4);
			UDR0 = (uint8_t)x_servo;
			while ( !( UCSR0A & (1<<UDRE0)) );
		
			
			ADMUX = 0b01000011;
			ADCSRA |=(1<<ADSC);
			while (( ADCSRA & (1<<ADSC)));
			y_servo = ( ADC/4);
			UDR0 =(uint8_t)y_servo;
			while ( !( UCSR0A & (1<<UDRE0)) );
			_delay_ms(10);    
    }
}

