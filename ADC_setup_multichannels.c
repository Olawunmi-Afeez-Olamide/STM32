#include "stm32g070xx.h"

uint16_t adc_values[4]; // Array to store ADC values

void GPIO_Config(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // Enable GPIOA clock
    GPIOA->MODER |= (GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3); // Set PA0, PA1, PA2, PA3 to analog mode
}

void ADC_Config(void) {
    RCC->APBENR2 |= RCC_APBENR2_ADCEN; // Enable the ADC clock

    ADC1->CR |= ADC_CR_ADVREGEN; // Enable the ADC voltage regulator
    for (volatile int i = 0; i < 10000; i++); // Wait for the regulator to stabilize

    ADC1->CR |= ADC_CR_ADCAL; // Start ADC calibration
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration is complete

    ADC1->CFGR1 &= ~ADC_CFGR1_RES; // Set ADC resolution to 12 bits
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; // Set ADC alignment to right-aligned

    ADC1->CHSELR |= (ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2 | ADC_CHSELR_CHSEL3); // Select channels 0, 1, 2, 3

    ADC1->CFGR1 |= ADC_CFGR1_CONT; // Enable continuous conversion mode
    ADC1->CR |= ADC_CR_ADEN; // Enable the ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready
}

void Start_ADC_Conversion(void) {
    ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
}

void Read_ADC_Results(uint16_t *results) {
    for (int i = 0; i < 4; i++) {
        while (!(ADC1->ISR & ADC_ISR_EOC)); // Wait until conversion is complete
        results[i] = (uint16_t)ADC1->DR; // Read the ADC conversion result
    }
}

int main(void) {
    GPIO_Config(); // Configure GPIO for ADC input
    ADC_Config(); // Configure ADC

    while (1) {
        Start_ADC_Conversion(); // Start ADC conversion
        Read_ADC_Results(adc_values); // Read the ADC values
        // Do something with adc_values
    }
}
