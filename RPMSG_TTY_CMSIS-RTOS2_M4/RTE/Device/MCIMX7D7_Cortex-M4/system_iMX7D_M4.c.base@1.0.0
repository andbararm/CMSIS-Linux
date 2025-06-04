/**************************************************************************//**
 * @file     system_iMX7D_M4.c
 * @brief    CMSIS Device System Source File for
 *           iMX7D_M4 Device
 * @version  V1.00
 * @date     05. February 2016
 ******************************************************************************/
/* Copyright (c) 2016 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdint.h>
#include "iMX7D_M4.h"


/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/*----------------------------------------------------------------------------
  SystemInit
 *----------------------------------------------------------------------------*/
void SystemInit (void) {

  extern uint32_t __Vectors;
  SCB->VTOR = (uint32_t) &__Vectors;

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

}

/*----------------------------------------------------------------------------
  SystemCoreClockUpdate Helper Functions
 *----------------------------------------------------------------------------*/
uint32_t get_SYS_PLL_Clock() {
  // Bypass enabled?
  if (CCM_ANALOG_PLL_480 & CCM_ANALOG_PLL_480_BYPASS_MASK) {
    switch ((CCM_ANALOG_PLL_480 & CCM_ANALOG_PLL_480_BYPASS_CLK_SRC_MASK) >> CCM_ANALOG_PLL_480_BYPASS_CLK_SRC_SHIFT) {
      case 0U: // Bypass with 24MHz Osc
        return 24U*1000U*1000U;
      case 1U: // External Clock Source CLK1P/N
        return DEFAULT_SYSTEM_CLOCK;
      case 2U: // External Clock Source CLK2
        return DEFAULT_SYSTEM_CLOCK;
      default: // Reserved
        return DEFAULT_SYSTEM_CLOCK;
    }
  } else {
    // DIV_SELECT enabled?
    if (CCM_ANALOG_PLL_480 & CCM_ANALOG_PLL_480_CLR_DIV_SELECT_MASK) {
      return 528U*1000U*1000U;
    } else {
      return 480U*1000U*1000U;
    }
  }
}

uint32_t get_ENET_PLL_Clock() {
  // Bypass enabled?
  if (CCM_ANALOG_PLL_ENET & CCM_ANALOG_PLL_ENET_BYPASS_MASK) {
    switch ((CCM_ANALOG_PLL_ENET & CCM_ANALOG_PLL_ENET_BYPASS_CLK_SRC_MASK) >> CCM_ANALOG_PLL_ENET_BYPASS_CLK_SRC_SHIFT) {
      case 0U: // Bypass with 24MHz Osc
        return 24U*1000U*1000U;
      case 1U: // External Clock Source CLK1P/N
        return DEFAULT_SYSTEM_CLOCK;
      case 2U: // External Clock Source CLK2
        return DEFAULT_SYSTEM_CLOCK;
      default: // Reserved
        return DEFAULT_SYSTEM_CLOCK;
    }
  } else {
    return 1000U*1000U*1000U; //1GHz
  }
}

/*----------------------------------------------------------------------------
  SystemCoreClockUpdate
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void) {
  uint32_t reg = CCM_TARGET_ROOT1;
  if (reg & CCM_TARGET_ROOT1_ENABLE_MASK) {
    uint32_t clock = 240U*1000U*1000U; // Default clock of M4

    // Get the current MUX value
    switch ((reg & CCM_TARGET_ROOT1_MUX_MASK) >> CCM_TARGET_ROOT1_MUX_SHIFT) {
      case 0U: // 24MHz Osc
        // Fixed Clock Source
        clock = 24U*1000U*1000U;
        break;
     
      case 1U: // SYS_PLL_DIV2
        clock = get_SYS_PLL_Clock() / 2U;
        break;
     
      case 2U: // ENET_PLL_DIV4
        clock = get_ENET_PLL_Clock() / 4U;
        break;
     
      case 3U: // SYS_PLL_PFD2
        // Get the 480MHz Clock and apply the PFD Fraction
        clock = (get_SYS_PLL_Clock() / ((CCM_ANALOG_PFD_480A & CCM_ANALOG_PFD_480A_PFD2_FRAC_MASK) >> CCM_ANALOG_PFD_480A_PFD2_FRAC_SHIFT) ) * 18;
        break;
     
      case 4U: // DDR_PLL_DIV2
        // Not implemented
        clock = DEFAULT_SYSTEM_CLOCK;
        break;
     
      case 5U: // AUDIO_PLL
        // Not implemented
        clock = DEFAULT_SYSTEM_CLOCK;
        break;
     
      case 6U: // VIDEO_PLL
        // Not implemented
        clock = DEFAULT_SYSTEM_CLOCK;
        break;
     
      case 7U: // USB_PLL
        // Fixed Clock Source
        clock = 480U*1000U*1000U;
        break;
    }

    // Apply the pre divider
    clock = clock / (((reg & CCM_TARGET_ROOT1_PRE_PODF_MASK) >> CCM_TARGET_ROOT1_PRE_PODF_SHIFT) + 1U);

    // Apply the post divider
    clock = clock / (((reg & CCM_TARGET_ROOT1_POST_PODF_MASK) >> CCM_TARGET_ROOT1_POST_PODF_SHIFT) + 1U);

    // Save the determined clock
    SystemCoreClock = clock;
    
  } else { //CCM not enabled... Core should not run!
    SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
  }
}
