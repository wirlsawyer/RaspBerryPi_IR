#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <time.h>

void NECsendUnit(int gpio_pin)
{
        clock_t start = clock();
	clock_t wait= clock();
	digitalWrite(gpio_pin, 0); 
	while(1)
        {
	        digitalWrite(gpio_pin, 1);
		// wait
		wait= clock();
		while (wait+8.5>clock())
		{		
			//85us	
			//nothing
		}		              
		digitalWrite(gpio_pin, 0);
		while (wait+26.5>clock())
		{	
			//26.5us		
			//nothing
		}	

		if (start+560<=clock())
			break;
    	}
	//digitalWrite(gpio_pin, 0); 
}
/*
void NECsend(int gpio_pin, long double timeout)
{
	timeout = 560+timeout;
        clock_t start = clock();
	NECsendUnit(gpio_pin);
	NECsendUnit(gpio_pin);
        digitalWrite(gpio_pin, 0); 
 	while(1)
        {                
		if (start+timeout<clock())
			break;
	}
}
*/
void NECsend(int gpio_pin, long double pulse, long double space)
{
	space = (pulse+space+1)*560;	

       clock_t start = clock();       
        int i=0;
        for (i=0; i<=pulse; i++)
        {
		NECsendUnit(gpio_pin);
        }	
        
 	while(1)
        {                
		if (start+space<clock())
			break;
	}
}

