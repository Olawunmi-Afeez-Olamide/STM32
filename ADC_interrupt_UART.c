#include "stm32g070xx.h"
#include "stdio.h"

uint16_t adc_value; // Declare a global variable to store the ADC value
void UART2_Init(void);
void UART2_Transmit_String(const char *str);

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

// Function to configure TIM3
void TIM3_Init(void)
{
    // Enable the clock for TIM3
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    // Set the prescaler to divide the clock (16 MHz / 16,000 = 1 kHz)
    TIM3->PSC = 16000 - 1; // Prescaler value
    
    // Set the auto-reload register to get 250ms interrupt (1 kHz / 4 = 250ms)
    TIM3->ARR = 1000 - 1; // Auto-reload value
	
    // Only counter overflow/underflow generates an update interrupt or DMA request if enabled
    TIM3->CR1 |= TIM_CR1_URS;

    // Enable the update interrupt
    TIM3->DIER |= TIM_DIER_UIE;
	
    // Generate an update event to reload the prescaler value immediately
    TIM3->EGR |= TIM_EGR_UG;

    // Enable the timer
    TIM3->CR1 |= TIM_CR1_CEN;

    // Enable TIM3 interrupt in NVIC
    NVIC_SetPriority(TIM3_IRQn, 0); // Set priority
    NVIC_EnableIRQ(TIM3_IRQn); // Enable interrupt
}

void GPIO_Init(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // Enable GPIOA clock
    RCC->APBENR2 |= RCC_APBENR2_ADCEN; // Enable the ADC clock
    GPIOA->MODER |= GPIO_MODER_MODE0; // Set PA0 to analog mode
}

void UART2_Init(void) {
    // Enable clock for GPIOA and USART2
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    
    // Configure PA2 as USART2_TX and PA3 as USART2_RX
    GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1); // Set to alternate function mode
    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos) | (1 << GPIO_AFRL_AFSEL3_Pos); // Set AF1 (USART2) for PA2 and PA3
    
    // Configure USART2
    USART2->BRR = 16000000 / 9600; // Assuming 16 MHz clock, set baud rate to 9600
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable transmitter and receiver
    USART2->CR1 |= USART_CR1_UE; // Enable USART2
}

void UART2_Transmit_String(const char *str) {
    while (*str) {
        while (!(USART2->ISR & USART_ISR_TXE_TXFNF)); // Wait until transmit data register is empty
        USART2->TDR = *str++; // Write data to the transmit data register
    }
}

void TIM3_IRQHandler(void)
{
    // Check if the update interrupt flag is set
    if (TIM3->SR & TIM_SR_UIF)
    {
        // Clear the update interrupt flag
        TIM3->SR &= ~TIM_SR_UIF;

        // Read the ADC value
        adc_value = ADC_Read();

        // Convert the ADC value to a string and transmit
        char adc_str[10];
        sprintf(adc_str, "ADC: %u\r\n", adc_value);
        UART2_Transmit_String(adc_str);
    }
}

int main(void) {
    GPIO_Init();
    ADC_Config();
    UART2_Init();
    TIM3_Init();

    while (1) {
        // Main loop
    }
}
