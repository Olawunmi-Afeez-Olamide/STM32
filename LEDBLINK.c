#include "stm32g0xx.h" // Include the device-specific header file

void GPIOA_Init(void) {
    // Enable the clock for GPIOA on the IOP bus
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // This sets the GPIOAEN bit in the IOPENR register

    // Configure PA5 as output
    GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk; // Clear mode bits for PA5
    GPIOA->MODER |= GPIO_MODER_MODE5_0;    // Set PA5 as output (01)
}

void LED_On(void) {
    GPIOA->ODR |= GPIO_ODR_OD5; // Set PA5 high
}

void LED_Off(void) {
    GPIOA->ODR &= ~GPIO_ODR_OD5; // Set PA5 low
}

int main(void) {
    GPIOA_Init(); // Initialize GPIOA

    while (1) {
        LED_On();  // Turn on LED2
        for (volatile int i = 0; i < 100000; i++) { } // Simple delay

        LED_Off(); // Turn off LED2
        for (volatile int i = 0; i < 100000; i++) { } // Simple delay
    }
}
