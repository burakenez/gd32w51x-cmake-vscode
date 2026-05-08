project(GD32W51x_F5HC_standard_peripheral LANGUAGES C CXX ASM)

# Comment-out unused source files.
add_library(GD32W51x_F5HC_standard_peripheral OBJECT
	${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_adc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_cau.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_cau_aes.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_cau_des.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_cau_tdes.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_crc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_ctc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_dbg.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_dci.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_dma.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_efuse.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_exti.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_fmc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_fwdgt.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_gpio.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_hau.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_hau_sha_md5.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_hpdf.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_i2c.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_icache.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_misc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_pkcau.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_pmu.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_qspi.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_rcu.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_rtc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_sdio.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_spi.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_sqpi.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_syscfg.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_timer.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_trng.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_tsi.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_tzpcu.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_usart.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Source/gd32w51x_f5hc_wwdgt.c
    )

target_include_directories(GD32W51x_F5HC_standard_peripheral PUBLIC
    ${DRIVERS_DIR}/GD32W51x_F5HC_standard_peripheral/Include
    )

# CMSIS header only library is linked.
target_link_libraries(GD32W51x_F5HC_standard_peripheral PUBLIC CMSIS)
