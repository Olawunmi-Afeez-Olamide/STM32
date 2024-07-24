#include "stm32g070xx.h"

void SysClockUpdate(void) {
    // Turn on the high-speed internal oscillator (HSI)
    RCC->CR |= RCC_CR_HSION;
    
    // Wait for HSI to be ready
    while (!(RCC->CR & RCC_CR_HSIRDY));
    
    // Configure the flash memory wait state
    FLASH->ACR &= ~(FLASH_ACR_LATENCY); // Clear latency bits
    FLASH->ACR |= (uint32_t)0x0;; // Enable 0 wait state for 24 MHz operation
    
    // Set HSI as PLL source
    RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;
    
    // Set PLLM to 1 (No division)
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLM_Pos);
    
    // Set PLLN to 8
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLN_Pos);
    
    // Set PLLR to 3 (division by 4)
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
    RCC->PLLCFGR |= (3 << RCC_PLLCFGR_PLLR_Pos);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable PLLR output
    
    // Set the AHB prescaler to 1 (no division), so HCLK = SYSCLK
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    
    // Turn on PLL
    RCC->CR |= RCC_CR_PLLON;
    
    // Wait for PLL to be ready
    while (!(RCC->CR & RCC_CR_PLLRDY));
    
    // Select PLL as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW; // Clear SW bits
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | (0x2 << RCC_CFGR_SW_Pos); // Select PLL as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != (0x2 << RCC_CFGR_SW_Pos)); // Wait for PLL to be used as system clock source
}

int main(void) {
    SysClockUpdate();
    // Your application code
}
