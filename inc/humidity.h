#ifndef HUMIDITY_H
#define HUMIDITY_H
#include <stdint.h>

void DHT11_Start (void);


uint8_t DHT11_Check_Response (void);

uint8_t DHT11_Read (void);

typedef struct
{
	uint8_t Temperature;
	uint8_t Humidity;
}DHT_DataTypedef;


void DHT11_GetData(DHT_DataTypedef *DHT_Data);
extern  sk_pin  sk_DHT11;

#endif 
