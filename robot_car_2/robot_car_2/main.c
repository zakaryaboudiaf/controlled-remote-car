


#define F_CPU 16000000UL


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define FOSC 16000000          // Clock Speed
#define BAUD 38400             // Define the boud rate of the USART
#define MYUBRR (FOSC/16/BAUD-1)


#define M1_backward PORTJ = 0b00000001 ;    //  L298N:   in1=LOW . in2=HIGH
#define M2_backward PORTH = 0b00000001 ;    //  L298N:   in3=LOW . in4=HIGH
#define M1_farward  PORTJ = 0b00000010 ;    //  L298N:   in1=HIGH. in2=LOW
#define M2_farward  PORTH = 0b00000010 ;    //  L298N:   in3=HIGH. in4=LOW


uint16_t x_axis = 0;
uint16_t y_axis = 0;
uint16_t x_servo = 0;
uint16_t y_servo = 0;
uint8_t channel = 8;

ISR (USART0_RX_vect)
{
    uint16_t value = UDR0;    // Receive the data from the remote control.
	
	switch(channel)
	{
	    case 8:
			x_axis = 4*value; // if channel = 8 store the received data on x-axis variable.
			channel = 9;      // change the channel to 9. 
		    break;
		
		case 9:
			y_axis = 4*value; // if channel = 9 store the received data on x-axis variable.
			channel = 10;     // change the channel to 10.
		    break;
			
		case 10:
			x_servo = 4*value; // if channel = 10 store the received data on x-axis variable.
			channel = 11;      // change the channel to 11.
		    break;
		
		case 11:
			y_servo =  4*value; // if channel = 11 store the received data on x-axis variable.
			channel = 8;        // change the channel to 8.
		    break;
			
		default:
			break;	
	}	
}

void PWM_motors_init(void)
{
	TCCR1A|= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM11);    //NON Inverted PWM
	TCCR1B|= (1<<WGM13) | (1<<WGM12) | (1<<CS10);       //PRESCALER=1 MODE 14(FAST PWM)
	ICR1 = 3999;                                        //fPWM=4KHZ of DC motors
}

void PWM_servo_init (void)
{
	TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);         //NON Inverted PWM
	TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30);  //PRESCALER=64 MODE 14(FAST PWM)
	ICR3=4999;                                          //fPWM=50Hz of servo	
}

void USART_Init (unsigned int ubrr )
{
	UBRR0H = (unsigned char)(ubrr>>8);                  // set baud rate
	UBRR0L = (unsigned char)ubrr;                       // set baud rate
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);                     // enable receiver and transmitter
	UCSR0C = (3<<UCSZ00);                               // set frame format: 8data, 1stop bit 
	UCSR0B |= (1<<RXCIE0);                              // interrupt enable of the USART                       
	sei();                                              // Enable the global interrupt
} 

int main(void)
{
	 DDRB |= 0b01100000;     // PB6 and PB5 : PWM of motors OUTPUTS
	 DDRE |= 0b00011000;     // PE4 and PE3 : PWM of servo  OUTPUTS
	 DDRH |= 0b00000011;     // PH1 and PH0 : IN3, IN4      OUTPUTS
	 DDRJ |= 0b00000011;     // PJ1 and PJ0 : IN1, IN2      OUTPUTS
	 
	 USART_Init (MYUBRR);
	 PWM_motors_init();
	 PWM_servo_init ();
	 
	 
	int16_t duty_1 = 0;
    int16_t duty_2 = 0;
	 
	 
    while (1) 
    {
		OCR3A = (-(0.3851*x_servo))+534;
		OCR3B = (0.3861*y_servo)+140;
		
		if ((y_axis <= 550) && (y_axis >= 470) && (x_axis <= 550) && (x_axis >= 470))
		{
			OCR1A = 0;
			OCR1B = 0;
		}
		if ((y_axis <= 550) && (y_axis >= 470) && (x_axis > 550))
		{
			M1_farward;
			M2_backward;
			OCR1A = ((8.46*x_axis) - 4650);
			OCR1B = ((8.46*x_axis) - 4650);
		}
		
		if ((y_axis <= 550) && (y_axis >= 470) && (x_axis < 470))
		{
			M1_backward;
			M2_farward;
			OCR1A = (3999-(8.5086*x_axis));
			OCR1B = (3999-(8.5086*x_axis));
			
		}
		
		if ((y_axis > 550) && (x_axis >= 470) && (x_axis <= 550))
		{
			M1_farward;
			M2_farward;
			OCR1A = ((8.46*y_axis) - 4650) ;
			OCR1B = ((8.46*y_axis) - 4650) ;
			
		}
		
		if ((y_axis < 470) && (x_axis >= 470) && (x_axis <= 550))
		{
			M1_backward;
			M2_backward;
			OCR1A = (3999 - (8.5086* y_axis));
			OCR1B = (3999 - (8.5086* y_axis));
			
		}
		
		if ((y_axis > 550) && (x_axis > 550))
		{
			M1_farward;
			M2_farward;
			
			duty_1 = ((8.46*y_axis) - 4650) + ((8.46*x_axis)-4650);
			duty_2 = ((8.46*y_axis) - 4650) - ((8.46*x_axis)-4650);
			
			if (duty_1 > 3999)
			{
				OCR1A = 3999;
			}
			else
			{
				OCR1A = duty_1;
			}
			if (duty_2 < 0)
			{
				OCR1B = 0;
			}
			else
			{
				OCR1B = duty_2;
			}
			
		}
		
		if ((y_axis > 550) && (x_axis < 470))
		{
			M1_farward;
			M2_farward;
			
			duty_1 = ((8.46*y_axis) - 4650) - (3999-(8.5086*x_axis));
			duty_2 = ((8.46*y_axis) - 4650) + (3999-(8.5086*x_axis));
			
			if ( duty_1 < 0)
			{
				OCR1A = 0;
			}
			else
			{
				OCR1A = duty_1;
			}
			if ( duty_2 > 3999)
			{
				OCR1B = 3999;
			}
			else
			{
				OCR1B = duty_2;
			}
			
		}
		
	/*	if ((y_axis < 470) && (x_axis > 550))
		{
			M1_backward;
			M2_backward;
			
			duty_1 = (3999 - (8.5086* y_axis)) + ((8.46*x_axis)-4650);
			duty_2 = (3999 - (8.5086* y_axis)) - ((8.46*x_axis)-4650);
			
			if ( duty_1 > 3999)
			{
				OCR1A = 3999;
			}
			else
			{
				OCR1A = duty_1;
			}
			if ( duty_2 < 0)
			{
				OCR1B = 0;
			}
			else
			{
				OCR1B = duty_2;
			}
		}
		
		if ((y_axis < 470) && (x_axis < 470))
		{
			M1_backward;
			M2_backward;
			
			duty_1 = (3999 - (8.5086* y_axis)) - (3999-(8.5086*x_axis));
			duty_2 = (3999 - (8.5086* y_axis)) + (3999-(8.5086*x_axis));
			
			if ( duty_1 < 0)
			{
				OCR1A = 0;
			}
			else
			{
				OCR1A = duty_1;
			}
			if ( duty_2 > 3999)
			{
				OCR1B = 3999;
			}
			else
			{
				OCR1B = duty_2;
			}
			
	    }   	 */
    }
}

