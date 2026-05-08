/*!
    \file    main.c
    \brief   ADC channel of temperature, vref and vbat demo

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
#include <stdio.h>
#include "gd32w515p_eval.h"

#if defined (GD32F5HC)
#define ADC_TEMP_CALIBRATION_VALUE_30           REG16(0x40022930)
int32_t temperature_value_30;
#endif /* defined (GD32F5HC) */
float temperature;
float vref_value;
float vbat_value;

void rcu_config(void);
void adc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* ADC configuration */
    adc_config();
    /* USART configuration */
    gd_eval_com_init(EVAL_COM0);
#if defined (GD32F5HC)
    temperature_value_30 = ADC_TEMP_CALIBRATION_VALUE_30 & 0x0FFF;
#endif /* defined (GD32F5HC) */
    while(1){
        /* delay a time in milliseconds */
        delay_1ms(2000U);

        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_INSERTED_CHANNEL);
        while(SET != adc_flag_get(ADC_FLAG_EOIC)){
        }
        adc_flag_clear(ADC_FLAG_EOIC);

        /* value convert */
#if defined (GD32F5HC)
        temperature = (temperature_value_30 - (int32_t)ADC_LDATA0)* 3.3f / 4096 / 4.1f * 1000 + 30 ;
        vref_value = (float)(ADC_LDATA1 * 3.3f / 4096);
        vbat_value = (float)((ADC_LDATA2 * 3.3f / 4096) * 4);
#else
        temperature = (float)((1.43f - ADC_IDATA0*3.3f/4096) * 1000 / 4.3f + 25);
        vref_value = (float)(ADC_IDATA1 * 3.3f / 4096);
        vbat_value = (float)((ADC_IDATA2 * 3.3f / 4096) * 4);
#endif /* defined (GD32F5HC) */
        
        printf("\r\n//***********************************//\r\n");
        printf(" the temperature data is %2.0f degrees Celsius\r\n", temperature);
        printf(" the reference voltage data is %5.3fV \r\n", vref_value);
        printf(" the battery voltage is %5.3fV\r\n", vbat_value);
        
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_HCLK_DIV6);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    /* ADC external trigger disable */
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    /* ADC channel length config */
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 3U);

    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(0, ADC_CHANNEL_9, ADC_SAMPLETIME_479POINT5);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(1, ADC_CHANNEL_10, ADC_SAMPLETIME_479POINT5);
    /* ADC 1/4 voltate of external battery config */
    adc_inserted_channel_config(2, ADC_CHANNEL_11, ADC_SAMPLETIME_479POINT5);
    /* ADC vbat channel enable */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VBAT, ENABLE);
#if defined (GD32F5HC)
    /* ADC temperature enable */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMPSENSOR, ENABLE);
    /* ADC vref enable */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VREFINT, ENABLE);
#else
    /* ADC temperature and vref enable */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMP_VREF, ENABLE);
#endif /* defined (GD32F5HC) */

    
    /* enable ADC interface */
    adc_enable();
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_INSERTED_CHANNEL);
}

