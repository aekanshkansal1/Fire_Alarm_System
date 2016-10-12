#include<avr/io.h>
#include<util/delay.h>
#define F_CPU 16000000UL
void adc_init()
{
ADMUX=0b01000000;
ADCSRA=((1<<ADEN)|(1<<ADPS1)|(1<<ADPS2)|(1<<ADPS0));
}

unsigned int adc_read(unsigned int channel)
{
ADMUX = 0b11000000|channel;
ADCSRA |= (1<<ADSC);
while(!(ADCSRA & (1<<ADIF)));
ADCSRA |= (1<<ADIF);
return(ADC); 
}

void timer_init()
{
TCCR0|=(1<<WGM01)|(1<WGM00)|(1<<COM00)|(1<<CS01);
DDRB|=(1<<PB3);
}


void main()
{
unsigned int value,temp,flag=0,i,j,st=0;
unsigned int a[3];             //for efficient algo take six values and more dalay.
DDRA=0x10001001;        //temperature sensor at PA0.IR at PA3.buzzer at PA7.
DDRB=0xff;             //speaker attached at PB3.LED attached at PORTB except PB3
//DDRC=0b00000001;       //relay attached at PC0
adc_init();
timer_init();
while(1)
{
    if(flag==3)          //algo storing last three values at interval of 10sec.
    flag=0;
	value=adc_read(0);    //reading value of temp sensor.
	value=value*500;
	temp=value/1024;      //converting it i degree.
	j=adc_read(3);
	if(j<290)
	{
     if(st>=1)
	 {
	 st=1;
	 PORTB|=0b00010000;   //led 4 glow on smoke
	 }
	}
	else
	{
     PORTB&=0b11101111;   //led 4 off on no smoke condition..
	 st=2;  
	}
	if(flag==0)
	{
	a[0]=temp;
	if(st==0)           //in starting a[1],a[2],a[3] are same.
	{
	a[1]=temp;
	a[2]=temp;
	}
	}
	else if(flag==1)
	a[1]=temp;
	else if(flag==2)
	a[2]=temp;
	flag++;	
	if(((temp-a[2]>5.5)||(temp-a[1]>5.5)||(temp-a[0]>5.5))&& st==1)
	{
	PORTB|=0b01000000;     //led 6 on.
//    PORTC|=0b00000001;    //bulb off.
	OCR0=130;
	PORTA|=0b10000000;      //buzzer on
	}
	else if(((temp-a[2]>4 && temp-a[2]<=5.5)||(temp-a[1]>4 && temp-a[1]<=5.5)||(temp-a[0]>4 && temp-a[0]<=5.5))&& st==1)
	{
	PORTB|=0b01000000;     //led 6 on.
//	PORTC|=0b00000001;    //bulb off.
	OCR0=90;
	PORTA|=0b10000000;      //buzzer on
	}
	else if(((temp-a[2]>2 && temp-a[2]<=4)||(temp-a[1]>2 && temp-a[1]<=4)||(temp-a[0]>2 && temp-a[0]<=4)) && st==1)
	{
	PORTB|=0b00100000;     //led 5 on.
//	PORTC|=0b00000001;    //bulb off.
	PORTB&=0b10111111;     //led 6 off
	OCR0=70;
	PORTA&=0b01111111;      //buzzer off
	}
	else if(((temp-a[2]>1 && temp-a[2]<=2)||(temp-a[1]>1 && temp-a[1]<=2)||(temp-a[0]>1 && temp-a[0]<=2)) && st==1)
	{
	PORTB|=0b00100000;     //led 5 on.
	PORTB&=0b10111111;     //led 6 off
 //  PORTC|=0b00000001;    //bulb off.
	OCR0=50;
	PORTA&=0b01111111;      //buzzer off
	}
	else
	{
	PORTB&=0b10011111;     //led 5,6 both off.
    OCR0=0;                //speaker off
	PORTA&=0b01111111;     //buzzer off
 //   PORTC|=0b00000000;     //bulb on.
	}

if(temp>22 && temp<=35)
{
PORTB|=0b00000001;          //led 0 on.
PORTB&=0b11111001;          //led 1,2 off
}
else if(temp>35 & temp<=42)
{
PORTB|=0b00000010;          //led 1 on.
PORTB&=0b11111010;          //led 0,2 off
}
else if(temp>42)
{
PORTB|=0b00000100;          //led 2 on.
PORTB&=0b11111100;          //led 0,1 off
}
else
{
PORTB&=0b11111000;          //0,1,2 off
}
for(i=0;i<10;i++)
_delay_ms(500);           //5 sec delay.
st++;
}
}
