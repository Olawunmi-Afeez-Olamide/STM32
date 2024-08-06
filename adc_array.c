#include "stm32g070xx.h"

uint16_t adc_values[25]; // Array to store ADC values
volatile uint8_t adc_index = 0; // Index to track the number of stored values

void TIM3_IRQHandler(void)
{
    // Check if the update interrupt flag is set
    if (TIM3->SR & TIM_SR_UIF)
    {
        // Clear the update interrupt flag
        TIM3->SR &= ~TIM_SR_UIF;

        // Start ADC conversion
        ADC1->CR |= ADC_CR_ADSTART;

        // Wait until conversion is complete
        while (!(ADC1->ISR & ADC_ISR_EOC));

        // Read the ADC conversion result and store it in the array
        adc_values[adc_index++] = (uint16_t)ADC1->DR;

        // Stop timer if 25 values are stored
        if (adc_index == 25) {
            adc_index=0; // Disable the timer
        }
    }
}

void GPIO_Config(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // Enable GPIOA clock
    GPIOA->MODER |= GPIO_MODER_MODE0; // Set PA0 to analog mode
}

void ADC_Config(void) {
    RCC->APBENR2 |= RCC_APBENR2_ADCEN; // Enable the ADC clock

    ADC1->CR |= ADC_CR_ADVREGEN; // Enable the ADC voltage regulator
    for (volatile int i = 0; i < 10000; i++); // Wait for the regulator to stabilize

    ADC1->CR |= ADC_CR_ADCAL; // Start ADC calibration
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration is complete

    ADC1->CFGR1 &= ~ADC_CFGR1_RES; // Set ADC resolution to 12 bits
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; // Set ADC alignment to right-aligned

    ADC1->CHSELR |= ADC_CHSELR_CHSEL0; // Select channels 0

    ADC1->CFGR1 &= ~ADC_CFGR1_CONT; // Disable continuous conversion mode
    ADC1->CR |= ADC_CR_ADEN; // Enable the ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready
}

void Timer_Config(void) {
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN; // Enable TIM3 clock

    TIM3->PSC = 16000 - 1; // Set prescaler to 16000 (assuming 16 MHz clock, this gives 1 ms tick)
    TIM3->ARR = 2000 - 1; // Set auto-reload to 2000 (2 seconds interval)
    TIM3->DIER |= TIM_DIER_UIE; // Enable update interrupt
    TIM3->CR1 |= TIM_CR1_CEN; // Enable the timer

    NVIC_EnableIRQ(TIM3_IRQn); // Enable TIM3 interrupt in NVIC
}

int main(void) {
    GPIO_Config(); // Configure GPIO for ADC input
    ADC_Config(); // Configure ADC
    Timer_Config(); // Configure timer for interrupts

    while (1) {
        // Main loop does nothing, ADC values are stored in the interrupt handler
    }
}
