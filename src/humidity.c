/***************************  DHT11 FUNCTIONS *************************************************/
#include "pin.h"
#include "tick.h"
#include "timer.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rtc.h>
#include <stdint.h>
#include "humidity.h"



sk_pin sk_DHT11 = {.port=SK_PORTD, .pin=11, .isinverse=false};

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM; uint8_t Presence = 0;

/// GPIO_PUPD_PULLUP

void DHT11_Start (void)
{
	
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11); // set pin as output
    gpio_clear(GPIOD, GPIO11);  // pull the pin low(0)
    sk_tick_delay_ms(18); // wait for 18 ms
    gpio_set(GPIOD,GPIO11); // [ull the pin high
    t_delay_us(30);  // wait for 30us
    gpio_mode_setup(GPIOD, 	GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);  //  set pin as input

}


uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	t_delay_us(40); // wait for 40us
	if (!(sk_pin_read(sk_DHT11)))
	{

			t_delay_us(80); // wait for 80us
			if ((sk_pin_read(sk_DHT11)))
				Response = 1;
			else 
				Response = -1; //255
	}

	while (sk_pin_read(sk_DHT11));   // wait for the pin to go low

	return Response;
}


uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(sk_pin_read(sk_DHT11)));   // wait for the pin to go high
		t_delay_us(40);   // wait for 40 us
		if (!(sk_pin_read(sk_DHT11)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((sk_pin_read(sk_DHT11)));  // wait for the pin to go low
	}
	return i;
}



void DHT11_GetData (DHT_DataTypedef *DHT_Data)
{
    DHT11_Start ();
	Presence = DHT11_Check_Response ();
	Rh_byte1 = DHT11_Read ();
	Rh_byte2 = DHT11_Read ();
	Temp_byte1 = DHT11_Read ();
	Temp_byte2 = DHT11_Read ();
	SUM = DHT11_Read();

	DHT_Data->Temperature = Temp_byte1;
	DHT_Data->Humidity = Rh_byte1;
}


