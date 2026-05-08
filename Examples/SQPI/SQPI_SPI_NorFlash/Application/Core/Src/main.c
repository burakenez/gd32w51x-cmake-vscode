/*!
    \file  main.c
    \brief SQPI SPI-FLASH example

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
#include "systick.h"
#include "gd32w515p_eval.h"
#include "sqpi_flash.h"

#if defined (GD32F5HC)
#define SFLASH_ID                0xC8401500U
#define BUFFER_SIZE              256U
#define FLASH_WRITE_ADDRESS      0x000000
#define FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];
uint32_t flash_id = 0U;
uint8_t  is_successful = 0U;
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);
void sqpi_gpio_config(void);

int main(void)
{
    uint32_t i = 0;
    /* configure systick */
    systick_config();
    /* initialize SQPI GPIO */
    sqpi_gpio_config();
    gd_eval_com_init(EVAL_COM0);
    /* read SPI flash ID */
    sqpi_flash_init();
    flash_id = sqpi_flash_read_id();
    if(flash_id != SFLASH_ID) {
        printf("\n\rRead the Flash_ID failed!\n\r");
        while(1);
    }
    printf("\n\rThe Flash_ID:0x%X\n\r\n\r", flash_id);
    /* enable flash quad operation */
    sqpi_flash_quad_enable();
    /* GPIO configure for quad SPI mode */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);

    /* flash id is correct */
    if(SFLASH_ID == flash_id) {
        printf("\n\rWrite to tx_buffer:\n\r\n\r");

        /* printf tx_buffer value */
        for(i = 0U; i < BUFFER_SIZE; i++) {
            tx_buffer[i] = i;
            printf("0x%02X ", tx_buffer[i]);

            if(15 == i % 16) {
                printf("\n\r");
            }
        }

        printf("\n\r\n\rRead from rx_buffer:\n\r\n\r");

        /* erase the specified flash sector */
        sqpi_flash_sector_erase(FLASH_WRITE_ADDRESS);

        /* write tx_buffer data to the flash */
        sqpi_flash_buffer_write(FLASH_WRITE_ADDRESS, tx_buffer, BUFFER_SIZE);

        delay_1ms(10U);
        /* read a block of data from the flash to rx_buffer */
        sqpi_flash_buffer_read(FLASH_READ_ADDRESS, rx_buffer, BUFFER_SIZE);

        /* printf rx_buffer value */
        for(i = 0U; i < BUFFER_SIZE; i ++) {
            printf("0x%02X ", rx_buffer[i]);
            if(15 == i % 16) {
                printf("\n\r");
            }
        }

        if(ERROR == memory_compare(tx_buffer, rx_buffer, BUFFER_SIZE)) {
            printf("\n\rErr:Data Read and Write aren't Matching.\n\r");
            is_successful = 1;
        }

        /* spi qspi flash test passed */
        if(0 == is_successful) {
            printf("\n\rSPI-GD25Q16 Test Passed!\n\r");
        }
    } else {
        /* spi flash read id fail */
        printf("\n\rSPI Flash: Read ID Fail!\n\r");
    }

    while(1);
}

/*!
    \brief      SQPI GPIO configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_gpio_config(void)
{
    /* enable SQPI GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_8, GPIO_PIN_3 | GPIO_PIN_4);

    /* configure SQPI GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_3 | GPIO_PIN_4);

    gpio_bit_set(GPIOB, GPIO_PIN_4);
    gpio_bit_reset(GPIOB, GPIO_PIN_3);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
#else
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    while(1){
    }
}
#endif /* GD32F5HC */
