project(CMSIS LANGUAGES C CXX ASM)

add_library(CMSIS INTERFACE)

target_include_directories(CMSIS INTERFACE
    ${DRIVERS_DIR}/CMSIS/
    ${DRIVERS_DIR}/CMSIS/GD/GD32W51x_F5HC/Include

	# Added directory of "gd32w51x_f5hc_libopt.h".
    ${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )
