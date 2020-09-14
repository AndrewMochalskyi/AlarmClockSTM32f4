#include "rtc.h"
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/rcc.h>



struct bcd_s
{
  uint8_t tens;
  uint8_t units;
};

static struct bcd_s rtc_dec_to_bcd(uint8_t dec)
{
  struct bcd_s bcd;
  bcd.tens = dec / 10;
  bcd.units = dec - bcd.tens * 10;
  return bcd;
}

static uint8_t rtc_bcd_to_dec(struct bcd_s bcd)
{
  return((uint8_t)(bcd.tens * 10 + bcd.units));
}

static uint32_t rtc_set_hour(uint8_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd(val);
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_TR_HT_MASK) << RTC_TR_HT_SHIFT);
  tmp |= ((bcd.units & RTC_TR_HU_MASK) << RTC_TR_HU_SHIFT);
  return tmp;
}

static uint32_t rtc_set_minute(uint8_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd(val);
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_TR_MNT_MASK) << RTC_TR_MNT_SHIFT);
  tmp |= ((bcd.units & RTC_TR_MNU_MASK) << RTC_TR_MNU_SHIFT);
  return tmp;
}

static uint32_t rtc_set_second(uint8_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd(val);
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_TR_ST_MASK) << RTC_TR_ST_SHIFT);
  tmp |= ((bcd.units & RTC_TR_SU_MASK) << RTC_TR_SU_SHIFT);
  return tmp;
}

/* Date Register contains only 1 byte for storing Year.
 * Year Register = (Year - 1970)
 */
static uint32_t rtc_set_year(uint16_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd((uint8_t)(val - 1970));
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_DR_YT_MASK) << RTC_DR_YT_SHIFT);
  tmp |= ((bcd.units & RTC_DR_YU_MASK) << RTC_DR_YU_SHIFT);
  return tmp;
}

static uint32_t rtc_set_week_day(uint8_t val)
{
  uint32_t tmp = 0;
  tmp |= ((val & RTC_DR_WDU_MASK) << RTC_DR_WDU_SHIFT);
  return tmp;
}

static uint32_t rtc_set_month(uint8_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd(val);
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_DR_MT_MASK) << RTC_DR_MT_SHIFT);
  tmp |= ((bcd.units & RTC_DR_MU_MASK) << RTC_DR_MU_SHIFT);
  return tmp;
}

static uint32_t rtc_set_day(uint8_t val)
{
  struct bcd_s bcd;
  bcd = rtc_dec_to_bcd(val);
  uint32_t tmp = 0;
  tmp |= ((bcd.tens & RTC_DR_DT_MASK) << RTC_DR_DT_SHIFT);
  tmp |= ((bcd.units & RTC_DR_DU_MASK) << RTC_DR_DU_SHIFT);
  return tmp;
}

static uint8_t rtc_get_hour(void)
{
  uint8_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_TR & (RTC_TR_HT_MASK << RTC_TR_HT_SHIFT)) >> 20);
  bcd.units = ((RTC_TR & (RTC_TR_HU_MASK << RTC_TR_HU_SHIFT)) >> 16);
  val = rtc_bcd_to_dec(bcd);
  return val;
}

static uint8_t rtc_get_minute(void)
{
  uint8_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_TR & (RTC_TR_MNT_MASK << RTC_TR_MNT_SHIFT)) >> 12);
  bcd.units = ((RTC_TR & (RTC_TR_MNU_MASK << RTC_TR_MNU_SHIFT)) >> 8);
  val = rtc_bcd_to_dec(bcd);
  return val;
}

static uint8_t rtc_get_second(void)
{
  uint8_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_TR & (RTC_TR_ST_MASK << RTC_TR_ST_SHIFT)) >> 4);
  bcd.units = ((RTC_TR & (RTC_TR_SU_MASK << RTC_TR_SU_SHIFT)) >> 0);
  val = rtc_bcd_to_dec(bcd);
  return val;
}

/* Date Register contains only 1 byte for storing Year.
 * Year Register = (Year - 1970)
 */
static uint16_t rtc_get_year(void)
{
  uint16_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_DR & (RTC_DR_YT_MASK << RTC_DR_YT_SHIFT)) >> 20);
  bcd.units = ((RTC_DR & (RTC_DR_YU_MASK << RTC_DR_YU_SHIFT)) >> 16);
  val = rtc_bcd_to_dec(bcd) + 1970;
  return val;
}

static uint8_t rtc_get_week_day(void)
{
  uint8_t val;
  val = ((RTC_DR & (RTC_DR_WDU_MASK << RTC_DR_WDU_SHIFT)) >> 13);
  return val;
}

static uint8_t rtc_get_month(void)
{
  uint8_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_DR & (RTC_DR_MT_MASK << RTC_DR_MT_SHIFT)) >> 12);
  bcd.units = ((RTC_DR & (RTC_DR_MU_MASK << RTC_DR_MU_SHIFT)) >> 8);
  val = rtc_bcd_to_dec(bcd);
  return val;
}

static uint8_t rtc_get_day(void)
{
  uint8_t val;
  struct bcd_s bcd;
  bcd.tens = ((RTC_DR & (RTC_DR_DT_MASK << RTC_DR_DT_SHIFT)) >> 4);
  bcd.units = ((RTC_DR & (RTC_DR_DU_MASK << RTC_DR_DU_SHIFT)) >> 0);
  val = rtc_bcd_to_dec(bcd);
  return val;
}

static void rcc_bdcr_clk_sel(uint8_t clock_sorce)
{
    uint32_t reg = RCC_BDCR;
    reg &= ~(RCC_BDCR_RTCSEL_MASK << RCC_BDCR_RTCSEL_SHIFT);
    reg |= clock_sorce << RCC_BDCR_RTCSEL_SHIFT;
    RCC_BDCR = reg;
}


static void rtc_set_clock_sorce(enum rcc_osc rtc_osc)
{
    rcc_osc_on(rtc_osc);
    rcc_wait_for_osc_ready(rtc_osc);
    switch(rtc_osc) {
    case RCC_LSE:
        // rcc_osc_on(RCC_LSE);
        // rcc_wait_for_osc_ready(RCC_LSE);
        // External low-speed oscillator enable
        RCC_BDCR |= RCC_BDCR_LSEON;
        // Wait for oscillator ready
        while (!(RCC_BDCR & RCC_BDCR_LSERDY));
        // Select clock sorce for RTC
        rcc_bdcr_clk_sel(RCC_BDCR_RTCSEL_LSE);
        break;
    // LSI oscillator clock used as the RTC clock
    case RCC_LSI:
        // rcc_osc_on(RCC_LSI);
        // rcc_wait_for_osc_ready(RCC_LSI);
        // Internal low-speed oscillator enable
        RCC_CSR |= RCC_CSR_LSION;
        // Wait for oscillator ready
        while (!(RCC_CSR & RCC_CSR_LSIRDY));
        // Select clock sorce for RTC
        rcc_bdcr_clk_sel(RCC_BDCR_RTCSEL_LSI);
        break;
    case RCC_HSE:
        // rcc_osc_on(RCC_LSE);
        // rcc_wait_for_osc_ready(RCC_LSE);
        // HSE clock enable
        RCC_CR |= RCC_CR_HSEON;
        // Wait for oscillator ready
        while (!(RCC_CR & RCC_CR_HSERDY));
        // Select clock sorce for RTC
        rcc_bdcr_clk_sel(RCC_BDCR_RTCSEL_HSE);
        break;
    default:
        rcc_bdcr_clk_sel(RCC_BDCR_RTCSEL_NONE);
    }
}

/*---------------------------------------------------------------------------*/
 /////@warning You must configure RCC peripheral for RTC and PWR

void rtc_calendar_config(enum rcc_osc rtc_osc)
{

	// Power interface clock enable 
	 rcc_periph_clock_enable(RCC_PWR);
    // Disable Backup Domain Write Protection.
	// This allows backup domain registers to be changed.
	// These registers are write protected after a reset.
	pwr_disable_backup_domain_write_protect();
	// Backup domain reset,
	RCC_BDCR |= RCC_BDCR_BDRST;
	RCC_BDCR &= ~RCC_BDCR_BDRST;
	rtc_set_clock_sorce(rtc_osc);
    /* Enable the RTC clock */
	rcc_periph_clock_enable(RCC_RTC); // ->RCC_BDCR |= RCC_BDCR_RTCEN;
    // Unlock write access to the RTC registers

  pwr_enable_backup_domain_write_protect();
}

/*---------------------------------------------------------------------------*/
//// Set calendar on the RTC register
////    @warning This function uses a time structure like in standard C

void rtc_calendar_set(struct time_s time)
{
  pwr_disable_backup_domain_write_protect();
  rtc_unlock();
  /* Allow to update Calendar value */
  RTC_ISR |= RTC_ISR_INIT;
  while(!(RTC_ISR & RTC_ISR_INITF));

  /* Set Time Register */
  /* Be careful : TR register must be setting in one time */
  uint32_t reg_tr = rtc_set_hour(time.hour)      +
                    rtc_set_minute(time.minute)  +
                    rtc_set_second(time.second);
  RTC_TR = reg_tr;
  /* Set Date Register */
  /* Be careful : DR register must be setting in one time */
  uint32_t reg_dr = rtc_set_year(time.year)          +
                    rtc_set_month(time.month)        +
                    rtc_set_week_day(time.week_day)  +
                    rtc_set_day(time.day);
  RTC_DR = reg_dr;

  RTC_CR &= ~RTC_CR_FMT; /* 24-hour format */

  /* Exit Initialization sequence */
  RTC_ISR &= ~RTC_ISR_INIT;
  rtc_lock();
  pwr_enable_backup_domain_write_protect();

  while(!(RTC_ISR & RTC_ISR_RSF)); /* Wait for allowing read date and time register */
}

/*---------------------------------------------------------------------------*/
///  Get the calendar on the RTC register
///    @warning This function uses a time structure like in standard C

struct time_s rtc_calendar_get(void)
{
  struct time_s tmp;

  while(RTC_CR & RTC_CR_BYPSHAD);
  tmp.hour     = rtc_get_hour();
  tmp.minute   = rtc_get_minute();
  tmp.second   = rtc_get_second();
  tmp.year     = rtc_get_year();
  tmp.month    = rtc_get_month();
  tmp.day      = rtc_get_day();
  tmp.week_day = rtc_get_week_day();

  return tmp;
}


 ///  Enable the wakeup timer
 ///  @warning You must unlock the registers before using this function

void rtc_enable_wakeup_timer(void)
{
	RTC_CR |= RTC_CR_WUTE | (RTC_CR_OSEL_WAKEUP << RTC_CR_OSEL_SHIFT);
	rtc_enable_wakeup_timer_interrupt();
}
/*---------------------------------------------------------------------------*/
///  Disable the wakeup timer
/// @warning You must unlock the registers before using this function

void rtc_disable_wakeup_timer(void)
{
	RTC_CR &= ~RTC_CR_WUTE;
	rtc_disable_wakeup_timer_interrupt();
}
/*---------------------------------------------------------------------------*/
///  Enable the wakeup timer interrupt
/// @warning You must unlock the registers before using this function

void rtc_enable_wakeup_timer_interrupt(void)
{
	/* FTFM:
	 * To enable the RTC Wakeup interrupt, the following sequence is
	 * required:
	 * 1. Configure and enable the EXTI Line 22 in interrupt mode and
	 *    select the rising edge sensitivity.
	 */
	exti_enable_request(EXTI22);
	exti_set_trigger(EXTI22, EXTI_TRIGGER_RISING);
	/* 2. Configure and enable the RTC_WKUP IRQ channel in the NVIC. */
	nvic_enable_irq(NVIC_RTC_WKUP_IRQ);
	nvic_set_priority(NVIC_RTC_WKUP_IRQ, 1);
	/* 3. Configure the RTC to generate the RTC wakeup timer event. */
	RTC_CR |= RTC_CR_WUTIE;   /* Enable the interrupt */
}
/*---------------------------------------------------------------------------*/
///  Disable the wakeup timer interrupt
/// @warning You must unlock the registers before using this function

void rtc_disable_wakeup_timer_interrupt(void)
{
	/* 1. Disable EXTI Line 22 */
	exti_disable_request(EXTI22);
	/* 2. Disable RTC_WKUP IRQ channel in the NVIC. */
	nvic_disable_irq(NVIC_RTC_WKUP_IRQ);
	/* 3. Disable RTC wakeup timer event. */
	RTC_CR &= ~RTC_CR_WUTIE;
}