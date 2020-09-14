#include <stdint.h>
#include <stdbool.h>
 
#ifndef _TIME_H
#define _TIME_H
 
    typedef struct
    {
        uint8_t Year;      // Год
        uint8_t Month;     // Месяц
        uint8_t Day;       // День месяца
        
        uint8_t DayOfWeek; // День недели
        
        uint8_t Hours;     // Часы
        uint8_t Minutes;   // Минуты
        uint8_t Seconds;   // Секунды
    } TDateTime;
 
    // Инициализация модуля
    void rtc_Init(void);
 
    // Сброс состояния часов
    void rtc_Reset(void);
    
    // Получить текущее время
    void rtc_Get(TDateTime * DateTime);
 
#endif