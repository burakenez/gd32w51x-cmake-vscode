/*!
    \file    main.c
    \brief   system clock switch, secure code with TZEN = 1

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
#include "systick.h"
#include "gd32w515p_eval.h"
#include "main.h"
#include "stdio.h"
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

#define TZBMPC1_BLOCK_NUMBER    255                    /* TZBMPC1 block number */

#define NONSECURE_START  ((uint32_t)0x08040000U)       /* nonsecure start address */
#define SEL_IRC16M      0x00U
#define SEL_HXTAL       0x01U
#define SEL_PLLP        0x02U
#define SEL_PLLDIG      0x03U

/* typedef for nonsecure callback functions */
typedef int32_t (*NonSecure_fpParam)(uint32_t) __attribute__((cmse_nonsecure_call));
typedef void    (*NonSecure_fpVoid) (void)     __attribute__((cmse_nonsecure_call));

#define HXTALSTB_DELAY_CFG     {                                 \
                                  volatile uint32_t i;           \
                                  for(i=0; i<0x2000; i++){       \
                                  }                              \
                               }

void tzbmpc_config();
void nonsecure_init();
void rcu_security_config();
void gpio_security_config();
CMSE_NS_ENTRY void secure_switch_system_clock_to_120m_hxtal(void);
#if defined (GD32F5HC)
CMSE_NS_ENTRY void secure_switch_system_clock_to_200m_irc16m(void);
#else
CMSE_NS_ENTRY void secure_switch_system_clock_to_180m_irc16m(void);
#endif
CMSE_NS_ENTRY uint32_t secure_systemclockget(void);
/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure TZBMPC */
    tzbmpc_config();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure systick */
    systick_config();
    /* configure rcu security */
    rcu_security_config();
    /* configure gpio security */
    gpio_security_config();

    /* setup and jump to non-secure */
    nonsecure_init();

    while(1){
    }
}

/*!
    \brief      configure tzbmpc
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tzbmpc_config()
{
    uint16_t block_number = 0U;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);

    /* SRAM1 is used to nonsecure code, so all blocks of SRAM1 should set to nonsecure */
    for(block_number = 0U; block_number <= TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      configure rcu security
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_security_config()
{
    /* configure AHBx/APBx prescaler bits security */
    rcu_security_enable(RCU_SEC_PRESCSEC);
    /* configure main PLL and status bits security */
    rcu_security_enable(RCU_SEC_PLLSEC);
    /* configure SYSCLK clock selection, STOPWUCK bit, clock output on MCO security */
    rcu_security_enable(RCU_SEC_SYSCLKSEC);
}

/*!
    \brief      configure gpio pin security
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_security_config()
{
    /* set GPIO pin secure */
    gpio_bit_reset_sec_cfg(TAMPER_WAKEUP_KEY_GPIO_PORT, TAMPER_WAKEUP_KEY_PIN);
    gpio_bit_reset_sec_cfg(LED1_GPIO_PORT, LED1_PIN);
    gpio_bit_reset_sec_cfg(LED2_GPIO_PORT, LED2_PIN);
    gpio_bit_reset_sec_cfg(LED3_GPIO_PORT, LED3_PIN);
}

/*!
    \brief      setup and jump to non-secure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_init()
{
    uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START)));
    NonSecure_fpVoid NonSecure_ResetHandler = (NonSecure_fpVoid)(*((uint32_t *)(NONSECURE_START + 4U)));
    /* set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* set non-secure stack pointer */
    __TZ_set_MSP_NS(NonSecure_StackPointer);
    /* start non-secure application */
    NonSecure_ResetHandler();
}

/*!
    \brief      switch system clock to 120M by HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void secure_switch_system_clock_to_120m_hxtal(void)
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
    /* Configure the main PLL, PSC = 40, PLL_N = 240, PLL_P = 2 */
    RCU_PLL = (25U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) );
#else
    /* Configure the main PLL, PSC = 40, PLL_N = 240, PLL_P = 2 */
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

#if  defined (GD32F5HC)
/*!
    \brief      switch system clock to 200M by IRC16M
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void secure_switch_system_clock_to_200m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    /* select IRC16M as system clock source, deinitialize the RCU */
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
    /* AHB = SYSCLK/2. AHB clock will be modified to SYSCLK after frequency switching finished to prevent Vcore fluctuations */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* Configure the main PLL, PSC = 16, PLL_N = 360, PLL_P = 2 */
    RCU_PLL = (16U | (400U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_IRC16M) );

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~(RCU_CFG0_SCS | RCU_CFG0_SCS_1);
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & (RCU_CFG0_SCSS | RCU_CFG0_SCSS_1))){
    }

    /* modify AHB clock to SYSCLK */
    RCU_MODIFY_UP_1(0x50U);
}
#else
/*!
    \brief      switch system clock to 180M by IRC16M
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void secure_switch_system_clock_to_180m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    /* select IRC16M as system clock source, deinitialize the RCU */
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

    /* Configure the main PLL, PSC = 16, PLL_N = 360, PLL_P = 2 */
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
#endif
/*!
    \brief      get the current systemclock
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY uint32_t secure_systemclockget(void)
{
    uint32_t sws;
    uint32_t pllpsc, plln, pllsel, pllp, ck_src, idx, clk_exp;
    uint32_t system_clock;

    /* exponent of AHB, APB1 and APB2 clock divider */
    const uint8_t ahb_exp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    sws = GET_BITS(RCU_CFG0, 2, 3);
    switch(sws){
    /* IRC16M is selected as CK_SYS */
    case SEL_IRC16M:
        system_clock = IRC16M_VALUE;
        break;
    /* HXTAL is selected as CK_SYS */
    case SEL_HXTAL:
        system_clock = HXTAL_VALUE;
        break;
    /* PLLP is selected as CK_SYS */
    case SEL_PLLP:
        /* get the value of PLLPSC[5:0] */
        pllpsc = GET_BITS(RCU_PLL, 0U, 5U);
        plln = GET_BITS(RCU_PLL, 6U, 14U);
        pllp = (GET_BITS(RCU_PLL, 16U, 17U) + 1U) * 2U;
        /* PLL clock source selection, HXTAL or IRC8M/2 */
        pllsel = (RCU_PLL & RCU_PLL_PLLSEL);
        if (RCU_PLLSRC_HXTAL == pllsel) {
            ck_src = HXTAL_VALUE;
        } else {
            ck_src = IRC16M_VALUE;
        }
        system_clock = ((ck_src / pllpsc) * plln)/pllp;
        break;
    /* IRC16M is selected as CK_SYS */
    default:
        system_clock = IRC16M_VALUE;
        break;
    }
    /* calculate AHB clock frequency */
    idx = GET_BITS(RCU_CFG0, 4, 7);
    clk_exp = ahb_exp[idx];
    system_clock = system_clock >> clk_exp;
    return system_clock;
}
