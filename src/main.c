#include "lcd_hd44780.h"
#include "tick.h"
#include "intrinsics.h"
#include "printf.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "timer.h"
#include "humidity.h"
#include "rtc.h"


static struct sk_lcd lcd = {
	.pin_group_data = &sk_io_lcd_data,
	.pin_rs = &sk_io_lcd_rs,
	.pin_en = &sk_io_lcd_en,
	.pin_rw = &sk_io_lcd_rw,
	.pin_bkl = &sk_io_lcd_bkl,
	.set_backlight_func = NULL,
	.delay_func_us = NULL,
	.delay_func_ms = &sk_tick_delay_ms,
	.is4bitinterface = true,
	.charmap_func = &sk_lcd_charmap_rus_cp1251
};


static void lcd_putstring(const char *str)
{
	for (char *cptr = str; *cptr != '\0'; cptr++)
		sk_lcd_putchar(&lcd, *cptr);
}






struct time_s time =
{
  .hour = 3,     /* hour : 0-23 */
  .minute = 59,   /* minute : 0-59 */
  .second = 0,   /* second : 0-59 */
  .year = 2020,     /* year (no limit of definition) */
  .month = 9,    /* month : January = 1 ... December = 12 */
  .day = 1,      /* day of the month : 1-31 */
  .week_day = 2 /* day of the week : Monday = 1 ... Sunday = 7 */
};



void print_time(struct time_s time)
{
    char tmp[20], buffer[20];

	if(time.hour < 10 )
		snprintf(tmp,sk_arr_len(tmp),"0%u",time.hour);
	else
		snprintf(tmp,sk_arr_len(tmp),"%u",time.hour);

	if(time.minute < 10)
		snprintf(tmp,sk_arr_len(tmp),"%s:0%u",tmp,time.minute);
	else
		snprintf(tmp,sk_arr_len(tmp),"%s:%u",tmp,time.minute);

	if(time.second < 10)
		snprintf(tmp,sk_arr_len(tmp),"%s:0%u",tmp,time.second);
	else
		snprintf(tmp,sk_arr_len(tmp),"%s:%u",tmp,time.second);





	snprintf(buffer, sk_arr_len(buffer), "%-5s  ", tmp);
	lcd_putstring(buffer);
}

void print_date(struct time_s time)
{
	char tmp[20], buffer[20];
	switch (time.week_day)
	{
		case 1:
			snprintf(tmp,sk_arr_len(tmp),"Mon,");
		break;
		case 2:
			snprintf(tmp,sk_arr_len(tmp),"Tue,");
		break;
		case 3:
			snprintf(tmp,sk_arr_len(tmp),"Wed,");
		break;
		case 4:
			snprintf(tmp,sk_arr_len(tmp),"Thu,");
		break;
		case 5:
			snprintf(tmp,sk_arr_len(tmp),"Fri,");
		break;
		case 6:
			snprintf(tmp,sk_arr_len(tmp),"Sat,");
		break;
		case 7:
			snprintf(tmp,sk_arr_len(tmp),"Sun,");
		break;
		default:
		snprintf(tmp,sk_arr_len(tmp),"Und,");
		break;
	}


switch (time.month)
	{
		case 1:
			snprintf(tmp,sk_arr_len(tmp),"%sJan ",tmp);
		break;
		case 2:
			snprintf(tmp,sk_arr_len(tmp),"%sFeb ",tmp);
		break;
		case 3:
			snprintf(tmp,sk_arr_len(tmp),"%sMar ",tmp);
		break;
		case 4:
			snprintf(tmp,sk_arr_len(tmp),"%sApr ",tmp);
		break;
		case 5:
			snprintf(tmp,sk_arr_len(tmp),"%sMay ",tmp);
		break;
		case 6:
			snprintf(tmp,sk_arr_len(tmp),"%sJun ",tmp);
		break;
		case 7:
			snprintf(tmp,sk_arr_len(tmp),"%sJul ",tmp);
		break;
		case 8:
			snprintf(tmp,sk_arr_len(tmp),"%sAug ",tmp);
		break;
		case 9:
			snprintf(tmp,sk_arr_len(tmp),"%sSep ",tmp);
		break;
		case 10:
			snprintf(tmp,sk_arr_len(tmp),"%sOct ",tmp);
		break;
		case 11:
			snprintf(tmp,sk_arr_len(tmp),"%sNov ",tmp);
		break;
		case 12:
			snprintf(tmp,sk_arr_len(tmp),"%sDec ",tmp);
		break;
		
	}


	if(time.day < 10 )
		snprintf(tmp,sk_arr_len(tmp),"%s0%u,",tmp,time.day);
	else
		snprintf(tmp,sk_arr_len(tmp),"%s%u,", tmp,time.day);

	snprintf(tmp,sk_arr_len(tmp),"%s%u", tmp,time.year);


		snprintf(buffer, sk_arr_len(buffer), "%s   ", tmp);
	lcd_putstring(buffer);
}




void print_temperature_and_humidity(uint8_t temperature, uint8_t hm )
{

   char buffer[20];

		
		snprintf(buffer, sk_arr_len(buffer), "T=%u H=%u      ", temperature, hm);
		lcd_putstring(buffer);

}

////////////////////////// rtc
int main(void)
{
	
	

	rcc_periph_clock_enable(RCC_GPIOE);		// LCD is connected to portE
	glsk_pins_init(false);

	sk_tick_init(16000000ul / 10000ul, 2);
	cm_enable_interrupts(); 

	sk_lcd_init(&lcd);
	sk_lcd_set_backlight(&lcd, 0xFF);
	sk_lcd_cmd_setaddr(&lcd, 0, false);
	

 /// button
sk_pin_set(sk_io_btn_mid,true);




		// rtc

 	rcc_periph_clock_enable(RCC_PWR);
  	rcc_periph_clock_enable(RCC_LSI);
  	rcc_periph_clock_enable(RCC_RTC);
  	rcc_periph_clock_enable(RCC_CSR);

  	rtc_calendar_config(RCC_LSI);
  	rtc_calendar_set(time);
  	struct time_s new_time;
  	new_time = rtc_calendar_get();


   /// last time to measure 
	uint8_t last_time = 0;	/// bad practice

// humidity


    t_delay_timer_init();
	rcc_periph_clock_enable(RCC_GPIOD);
	DHT_DataTypedef DHT_Data;


	
	
	////////////////
	
	while (1) {
			new_time = rtc_calendar_get();
     	//// top line
    	sk_lcd_cmd_setaddr(&lcd, 0, false);
		
    	

    	
			print_date(time);
		


 

		// down line
        sk_lcd_cmd_setaddr(&lcd, 0x40, false);
		if(sk_pin_read(sk_io_btn_mid))
    	{
		    DHT11_GetData(&DHT_Data);
			print_temperature_and_humidity(DHT_Data.Temperature, DHT_Data.Humidity);
			sk_tick_delay_ms(3000);
		
    	}
		else
		{
			print_time(new_time);
		}
		
		
	}
}


