#include "stm32g070xx.h"

// Function to configure TIM3
void TIM3_Init(void)
{
    // Enable the clock for TIM3
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    // Set the prescaler to divide the clock (16 MHz / 16,000 = 1 kHz)
    TIM3->PSC = 16000 - 1; // Prescaler value
    
    // Set the auto-reload register to get 250ms interrupt (1 kHz / 4 = 250ms)
    TIM3->ARR = 250 - 1; // Auto-reload value
	
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
    // Enable clock for GPIOA
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    // Set PA5 as output (assuming LED connected to PA5)
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk);
    GPIOA->MODER |= GPIO_MODER_MODE5_0; // Set PA5 to output mode
}

void Toggle_LED(void)
{
    GPIOA->ODR ^= GPIO_ODR_OD5; // Toggle PA5
}

void TIM3_IRQHandler(void)
{
    // Check if the update interrupt flag is set
    if (TIM3->SR & TIM_SR_UIF)
    {
        // Clear the update interrupt flag
        TIM3->SR &= ~TIM_SR_UIF;

        // Toggle the LED
        Toggle_LED();
    }
}

int main(void)
{
    // System Initialization
    SystemInit();

    // Initialize GPIO
    GPIO_Init();

    // Initialize TIM3
    TIM3_Init();

    // Main loop
    while (1)
    {
        // Your main code
    }
}
