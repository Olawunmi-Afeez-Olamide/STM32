#include "stm32g070xx.h" // Include the device-specific header file

uint16_t adc_value; // Declare a global variable to store the ADC value

void GPIO_Config(void) {
    GPIOA->MODER |= GPIO_MODER_MODE0; // Set PA0 to analog mode
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // Enable GPIOA clock
    RCC->APBENR2 |= RCC_APBENR2_ADCEN; // Enable the ADC clock
}

void ADC_Config(void) {
    ADC1->CR |= ADC_CR_ADVREGEN; // Enable the ADC voltage regulator
    for (volatile int i = 0; i < 10000; i++); // Wait for the regulator to stabilize

    ADC1->CR |= ADC_CR_ADCAL; // Start ADC calibration
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration is complete

    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; // Clear ADC clock mode configuration
    ADC1->CFGR1 &= ~ADC_CFGR1_RES; // Set ADC resolution to 12 bits
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; // Set ADC alignment to right-aligned

    ADC1->CHSELR |= ADC_CHSELR_CHSEL0; // Select ADC channel 0 (PA0)

    ADC1->CR |= ADC_CR_ADEN; // Enable the ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready
}

uint16_t ADC_Read(void) {
    ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
    while (!(ADC1->ISR & ADC_ISR_EOC)); // Wait until conversion is complete
    return (uint16_t)ADC1->DR; // Return the ADC conversion result
}

int main(void) {
    // SystemClock_Config(); // Configure the system clock (commented out)
    GPIO_Config(); // Configure GPIO for ADC input
    ADC_Config(); // Configure ADC

    while (1) {
        adc_value = ADC_Read(); // Read the ADC value
        // Do something with adc_value
    }
}
