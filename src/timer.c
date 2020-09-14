#include "timer.h"
#include "pin.h"
#include <libopencm3/cm3/sync.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


// quick example without overgeneralization
void t_delay_timer_init(void)
{
	rcc_periph_clock_enable(RCC_TIM7);

	// assume timer block is clocked @ 16 MHz
	// and we want 1 us tick time as delay function has 1 us resolution
	timer_set_prescaler(TIM7, 16 - 1);
	//timer_enable_preload(TIM7);
	timer_disable_preload(TIM7);
	timer_one_shot_mode(TIM7);

	// Set URS bit to update only at overflow (when auto-reload value is reached)
	timer_update_on_overflow(TIM7);

	timer_enable_irq(TIM7, TIM_DIER_UIE);
	timer_clear_flag(TIM7, TIM_SR_UIF);		// just in case

	nvic_set_priority(NVIC_TIM7_IRQ, 2);
	nvic_enable_irq(NVIC_TIM7_IRQ);
}


void t_delay_us(uint16_t val)
{
	// return fast
	if (!val)
		return;

	timer_set_period(TIM7, val);
	// generate update event to transfer ARPE contents to shadow register
	timer_generate_event(TIM7, TIM_EGR_UG);

	__dmb(); // data memory barrier (asm instuction DMB)
	// DMB acts as a data memory barrier. It ensures that all explicit memory accesses that
	// appear, in program order, before the DMB instruction are completed before any explicit
	// memory accesses that appear, in program order, after the DMB instruction. DMB does not
	// affect the ordering or execution of instructions that do not access memory.

	timer_enable_counter(TIM7);
	// __asm__ volatile ("isb");
	__asm__ volatile ("wfi");	// Enter sleep mode. WFI -- wait for interrupt
}


void t_delay_ms(uint32_t val)
{
	while (val--)
	delay_us(1000);
}

void tim7_isr(void)
{
	timer_clear_flag(TIM7, TIM_SR_UIF);
}
