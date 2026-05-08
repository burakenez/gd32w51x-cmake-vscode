/*!
    \file    main.c
    \brief   system clock switch example

    \version 2026-04-15, V2.0.0, firmware for gd32w51x_f5hc
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32w51x_f5hc.h"
#include <stdio.h>
#include "gd32w515p_eval.h"

/* The following is to prevent Vcore fluctuations caused by frequency switching.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
#define RCU_MODIFY_DE_2(__delay)  do{                                     \
                                      volatile uint32_t i,reg;            \
                                      if(0 != __delay){                   \
                                          /* Insert a software delay */   \
                                          for(i=0; i<__delay; i++){       \
                                          }                               \
                                          reg = RCU_CFG0;                 \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV2;     \
                                          /* AHB = SYSCLK/2 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                          reg = RCU_CFG0;                \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV4;     \
                                          /* AHB = SYSCLK/4 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                      }                                  \
                                  }while(0)

/* The following is to prevent Vcore fluctuations caused by frequency switching.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
#define RCU_MODIFY_UP_1(__delay)    do{                                     \
                                        volatile uint32_t i,reg;            \
                                        if(0U != (__delay)){                \
                                            /* Insert a software delay */   \
                                            for(i=0U; i<(__delay); i++){    \
                                            }                               \
                                            reg = RCU_CFG0;                 \
                                            reg &= ~(RCU_CFG0_AHBPSC);      \
                                            reg |= RCU_AHB_CKSYS_DIV1;      \
                                            RCU_CFG0 = reg;                 \
                                            /* Insert a software delay */   \
                                            for(i=0U; i<(__delay); i++){    \
                                            }                               \
                                        }                                   \
                                    }while(0U)

static void _delay(uint32_t timeout);
static void switch_system_clock_to_120m_hxtal(void);
static void switch_system_clock_to_180m_irc16m(void);
/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}

#if defined (GD32W515)
#define HXTALSTB_DELAY_CFG     {                                 \
                                  volatile uint32_t i;           \
                                  for(i=0; i<0x2000; i++){       \
                                  }                              \
                               }
#endif /* defined (GD32W515PI) || defined (GD32W515TX) */

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM0);
    printf("\r\nCK_SYS switch test demo\r\n");

    /* disable the USART */
    usart_disable(EVAL_COM0);

    /* switch system clock to 120MHz by HXTAL */
    switch_system_clock_to_120m_hxtal();
    gd_eval_com_init(EVAL_COM0);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    _delay(1000);

    /* switch system clock to 180MHz by IRC8M */
    switch_system_clock_to_180m_irc16m();
    gd_eval_com_init(EVAL_COM0);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    while(1){
    }
}

/*!
    \brief      delay function
    \param[in]  timeout: time out
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t timeout)
{
    __IO uint32_t i,j;
    for(i=0; i<timeout; i++){
        for(j=0; j<500; j++){
        }
    }
}

/*!
    \brief      switch system clock to 120M by HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_120m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;

    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC16M);
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    _soft_delay_(200);
    rcu_deinit();

#if defined (GD32F5HC)
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
#else
    /* power up the HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    RCU_CTL |= RCU_CTL_HXTALENPLL;
    HXTALSTB_DELAY_CFG
    RCU_CTL |= RCU_CTL_HXTALREADY;
#endif

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL0 |= PMU_CTL0_LDOVS;

    /* HXTAL is stable */
    /* reset the RCU_CFG0 AHBPSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_AHBPSC;
    /* reset the RCU_CFG0 APB1PSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_APB1PSC;
    /* reset the RCU_CFG0 APB2PSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_APB2PSC;

#if  defined (GD32F5HC)
    /* AHB = SYSCLK/2. AHB clock will be modified to SYSCLK after frequency switching finished to prevent Vcore fluctuations */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
#else
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
#endif
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

#if defined (GD32F5HC)
    /* configure the main PLL, PSC = 25, PLL_N = 240, PLL_P = 2 */
    RCU_PLL = (25U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) );
#else
    /* configure the main PLL, PSC = 40, PLL_N = 240, PLL_P = 2 */
    RCU_PLL = (40U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) );
#endif
    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

#if defined (GD32F5HC)
    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~(RCU_CFG0_SCS | RCU_CFG0_SCS_1);
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & (RCU_CFG0_SCSS | RCU_CFG0_SCSS_1))){
    }
#else
    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
#endif

#if  defined (GD32F5HC)
    /* modify AHB clock to SYSCLK */
    RCU_MODIFY_UP_1(0x50U);
#endif
}

/*!
    \brief      switch system clock to 180M by IRC16M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_180m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;

    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC16M);
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    _soft_delay_(200);
    rcu_deinit();
    /* enable IRC16M */
    RCU_CTL |= RCU_CTL_IRC16MEN;

    /* wait until IRC16M is stable or the startup time is longer than IRC16M_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC16MSTB);
    }while((0U == stab_flag) && (IRC16M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_IRC16MSTB)){
        while(1){
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL0 |= PMU_CTL0_LDOVS;

    /* IRC16M is stable */
    /* reset the RCU_CFG0 AHBPSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_AHBPSC;
    /* reset the RCU_CFG0 APB1PSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_APB1PSC;
    /* reset the RCU_CFG0 APB2PSC bits */
    RCU_CFG0 &= ~ RCU_CFG0_APB2PSC;

#if  defined (GD32F5HC)
    /* AHB = SYSCLK/2. AHB clock will be modified to SYSCLK after frequency switching finished to prevent Vcore fluctuations */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
#else
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
#endif
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* configure the main PLL, PSC = 16, PLL_N = 360, PLL_P = 2 */
    RCU_PLL = (16U | (360U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_IRC16M) );

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

#if defined (GD32F5HC)
    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~(RCU_CFG0_SCS | RCU_CFG0_SCS_1);
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & (RCU_CFG0_SCSS | RCU_CFG0_SCSS_1))){
    }
#else
    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
#endif

#if  defined (GD32F5HC)
    /* modify AHB clock to SYSCLK */
    RCU_MODIFY_UP_1(0x50U);
#endif
}
