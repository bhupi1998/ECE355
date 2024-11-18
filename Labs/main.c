//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

volatile int edge_flag_FG=0;
volatile int edge_flag_555=0;
volatile int inSig = 1;  //default input from signal generator PB changes to 0 for 555 input




// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void); //PA IO Setup
void myTIM2_Init(void);
void myEXTI_Init(void);
void myADC_Init(void); // initialize ADC for Potentiometer Reading
void myDAC_Init(void); // Initialize DAC




// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

 void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/


int
main(int argc, char* argv[])
{
	volatile uint16_t potValRaw;
	volatile uint16_t potValResistance;
	volatile float potValVoltage;

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab by Micah And Bhupinder...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */



	while (1)
	{
		// Start ADC Process,
		ADC1->CR |= ADC_CR_ADSTART;
		// Wait for EOC. Autoclears once ADC_DR is read
		while(!(ADC1->ISR & ADC_ISR_EOC )){};
		potValRaw = ADC1->DR;
		// Voltage value being read by DAC
		potValVoltage=(potValRaw/4095)*3.3;
		// Potentiometer Resistance
		potValResistance=1.22*potValRaw;
	}

	return 0;

}
/*
Set up ADC Operation on PA5
ADC setup is reset on powerup.
Setup operation:
	1-Enable clock for PA and ADC
	2-PA[5] Set to Analog mode
	3-Configure ADC_CHSELR[5] to 1 to select PA5 as a channel
	4-Configure SMPR to 111, slowest sample rate
	5-Configure CR[0]=1 to enable ADC
	6-Wait for ISR[0] = 1
*/
void myADC_Init(){
	// 1
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
    // This turns on the clock to PortA so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Enable ADC clock. BIT9 set to 1
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	// Set PA[5] to Analog Mode. Set to 11 for analog mode
	GPIOA->MODER |= GPIO_MODER_MODER5;
	// Configure ADC Register
	// 12 bit resolution, right align, overrun mode,continous mode
	ADC1->CFGR1 |= ADC_CFGR1_OVRMOD + ADC_CFGR1_CONT;
	ADC1->CFGR1 &= ~(ADC_CFGR1_RES+ADC_CFGR1_ALIGN);
	// Selecting channel 5
	ADC1->CJSELR = ADC_CHSELR_CHSEL5;    
	// Select ADC Sampling time
	ADC1->SMPR = ADC_SMPR_SMP; 
	// Enable ADC
	ADC1->CR |= ADC_CR_ADEN;
	// Wait for ADRDY Flag, indicating ADC is ready for operation
	while(!(ADC1->ISR & ADC_ISR_ADRDY)){};
}

// DON'T TRUST
void myDAC_Init(){
	// Set PA[4] to Analog Mode. Set to 11 for analog mode
	GPIOA->MODER |= GPIO_MODER_MODER4;
}
void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
    // This turns on the clock to PortA so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //User button PA0 configuration
	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
	/* Ensure no pull-up/pull-down for PA0 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

    // PA1 configuration - 555 timer
	/* Configure PA1 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);
	/* Ensure no pull-up/pull-down for PA1 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	/* Configure PA2 as input */
	// Relevant register: GPIOA->MODER
    // GPIO_MODER_MODER2 = 00110000
    GPIOA->MODER &= ~(GPIO_MODER_MODER2);
	/* Ensure no pull-up/pull-down for PA2 */
    // set to 00 for no pull ups
	// Relevant register: GPIOA->PUPDR
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

    //need PA 4 Analog output

    //need PA 5 Analog input

}


void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	// Relevant register: TIM2->CR1
    // Set to 0b10001100
    TIM2->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER
			;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
    TIM2->EGR = ((uint16_t)0x0001);
	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
    NVIC_SetPriority(TIM2_IRQn, 0);
	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
    NVIC_EnableIRQ(TIM2_IRQn);
	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
    TIM2->DIER |= TIM_DIER_UIE;
}


void myEXTI_Init()
{
	///EXTI2 SETUP
	//EXTI2_3 SETUP (done for Lab 2)
	/* Map EXTI2 line to PA2 */
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] &= ~(0xF<<8) ;
	/* EXTI2 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2; // enable rising edge for PA2
	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	// set EXTI IMR bit 2 (...43210) to 1 to unmask
	EXTI->IMR |= EXTI_IMR_MR2;
	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	// set timer2 priority to 0 (highest priority)
///CHANGED BELOW FROM "TIM2_IRQn" TO "EXTI2_3_IRQn" FOR THE PRIORITY SETTING
	NVIC_SetPriority(EXTI2_3_IRQn, 0);
	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	//interrupt set enable register -> EXTI2 (bit 6) -> 1 enables
	NVIC_EnableIRQ(EXTI2_3_IRQn);
	///EXTI0 setup
	//EXTI0_1 SETUP (done for Project)
	/* Map EXTI0 line to PA0 */
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] &= ~(0xF) ;
	/* EXTI0 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR0; // enable rising edge for PA0
	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	// set EXTI IMR bit 0 (...43210) to 1 to unmask
	EXTI->IMR |= EXTI_IMR_MR0;
	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	// set timer2 priority to 0 (highest priority)
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	/* Enable EXTI0 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	//interrupt set enable register -> EXTI0 (bit 6) -> 1 enables
	NVIC_EnableIRQ(EXTI0_1_IRQn);
///EXTI1SETUP
	/* Map EXTI1 line to PA1 */
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] &= ~(0xF<<4) ;
	/* EXTI0 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR1; // enable rising edge for PA0
	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	// set EXTI IMR bit 0 (...43210) to 1 to unmask
	EXTI->IMR |= EXTI_IMR_MR1;

}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR (status register, clear ^^Flag)
		TIM2->SR &= ~(TIM_SR_UIF);
		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}
// declared elsewhere - system/src/cmsis/vectors_stm32f051x8.c
void EXTI0_1_IRQHandler()
{
	//reset edge flags to prevent carry over errors
	edge_flag_FG = 0;
	edge_flag_555 = 0;

	//logic for EXI
	if((EXTI->PR & EXTI_PR_PR1)!=0){

		//add code for freq measuring when inSig ==
		if(inSig==0){
			//add code to measure EXTI1 Freq
		}
	}
	// logic for button press

	if((EXTI->PR & EXTI_PR_PR0)!=0){
		if(inSig ==0){
			inSig = 1;
			//Disable EXTI1 interrupt
			//mask interrupts from EXTI0 line
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 1 (...43210) to 0 to mask
			EXTI->IMR &= ~EXTI_IMR_MR1;
			//Enable EXTI2 interrupt
			/* Unmask interrupts from EXTI2 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 2 (...43210) to 1 to unmask
			EXTI->IMR |= EXTI_IMR_MR2;
		}else
		{inSig = 0;
			//Disable EXTI2 interrupt
			/* mask interrupts from EXTI2 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 2 (...43210) to 0 to mask
			EXTI->IMR &= ~EXTI_IMR_MR2;
			//Enable EXTI1 interrup
			/* Unmask interrupts from EXTI0 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 0 (...43210) to 1 to unmask
			EXTI->IMR |= EXTI_IMR_MR1;
		}

	}
		//test print
		///*NEED TO DISABLE PRINTS IN EXTI2_3
		trace_printf("PA 0 interrupt works\n");
	
		// Clear EXTI0 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		EXTI->PR |= EXTI_PR_PR0;

}



/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...
	volatile unsigned int count=0; // variable to save counter value
	float freq, period;


	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{
		//
		// 1. If this is the first edge:
		if(edge_flag_FG == 0){
			// set edge flag to 1
			edge_flag_FG = 1;
			//	- Clear count register (TIM2->CNT).
			TIM2->CNT = 0x0;
			//	- Start timer (TIM2->CR1).
			TIM2->CR1 |= (0x1);

		//    Else (this is the second edge):
		} else
		{
			//	- Stop timer (TIM2->CR1).
			TIM2->CR1 &= ~(0x1);

			// clear flag to prepare for next period
			edge_flag_FG = 0;

			//	- Read out count register (TIM2->CNT).
			count = TIM2->CNT;
			//	- Calculate signal period and frequency.
			period = (float)count / (float)SystemCoreClock;
			freq = 1/period;
			//	- Print calculated values to the console.
			//	  NOTE: Function trace_printf does not work
			//	  with floating-point numbers: you must use
			//	  "unsigned int" type to print your signal
			//	  period and frequency.
			trace_printf("FG Period is: %f ms     -   FG Freq is: %f kHz\n", (float) period*1000,(float) freq/1000);

		}
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		EXTI->PR |= EXTI_PR_PR2;

		}
	}





#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
