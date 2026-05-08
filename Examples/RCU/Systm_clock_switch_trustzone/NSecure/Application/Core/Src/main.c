/*!
    \file    main.c
    \brief   system clock switch, non-secure code with TZEN = 1

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

#define DELAY_1S       1000U
#define DELAY_500MS    500U

uint8_t swtich_flag = 0U;
uint32_t delay_times = 1000U;
uint32_t system_clock = 0U;

/* switch system clock to 120M  function */
extern void secure_switch_system_clock_to_120m_hxtal(void);
#if defined (GD32F5HC)
/* switch system clock to 200M  function */
extern void secure_switch_system_clock_to_200m_irc16m(void);
#else
/* switch system clock to 180M  function */
extern void secure_switch_system_clock_to_180m_irc16m(void);
#endif
/* get system clock function */
extern uint32_t secure_systemclockget(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* get current systemcore clock*/
    system_clock = secure_systemclockget();
    systick_config();
    /* initialize the LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    /* configure the key */
    gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_EXTI);

    while(1){
        if(swtich_flag == 1U){
#if defined (GD32F5HC)
            if(system_clock == 200000000U){
#else
            if(system_clock == 180000000U){
#endif
                 /* switch system clock to 120M */
                 secure_switch_system_clock_to_120m_hxtal();
                 /* update the SystemCoreClock with current core clock retrieved from cpu registers */
                 system_clock = secure_systemclockget();
                 /* configure systick */
                 systick_config();
                 /* modify delay_times */
                 delay_times = DELAY_500MS;
                 gd_eval_led_on(LED2);
                 gd_eval_led_off(LED3);
            }else if(system_clock == 120000000U){
#if defined (GD32F5HC)
                 /* switch system clock to 200M */
                 secure_switch_system_clock_to_200m_irc16m();
#else
                 /* switch system clock to 180M */
                 secure_switch_system_clock_to_180m_irc16m();
#endif
                 /* update the SystemCoreClock with current core clock retrieved from cpu registers */
                 system_clock = secure_systemclockget();
                 /* configure systick */
                 systick_config();
                 /* modify delay_times */
                 delay_times = DELAY_1S;
                 gd_eval_led_on(LED3);
                 gd_eval_led_off(LED2);
            }
            swtich_flag = 0U;
        }
        /* toggle LED1 */
        gd_eval_led_toggle(LED1);
        delay_1ms(delay_times);
    }
}
