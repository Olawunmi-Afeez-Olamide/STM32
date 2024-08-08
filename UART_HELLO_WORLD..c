   #include "stm32g070xx.h"

void UART2_Init(void);
void UART2_Transmit(char ch);
void UART2_Transmit_String(const char *str);
char UART2_Receive(void);

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

void UART2_Transmit(char ch) {
    // Wait until transmit data register is empty
    while (!(USART2->ISR & USART_ISR_TXE_TXFNF));
    // Write data to the transmit data register
    USART2->TDR = ch;
}

void UART2_Transmit_String(const char *str) {
    while (*str) {
        UART2_Transmit(*str++);
    }
}

char UART2_Receive(void) {
    while (!(USART2->ISR & USART_ISR_RXNE_RXFNE)); // Wait until data is received
    return (char)USART2->RDR;
}

int main(void) {
    UART2_Init();
    
    UART2_Transmit_String("Hello world\r\n"); // Send "Hello world" to PuTTY
    
    while (1) {
        // You can add more functionality here if needed
    }
}
