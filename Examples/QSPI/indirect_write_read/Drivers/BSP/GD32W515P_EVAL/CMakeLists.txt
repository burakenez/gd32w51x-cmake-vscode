project(GD32W515P_EVAL LANGUAGES C CXX ASM)

add_library(GD32W515P_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32W515P_EVAL/gd32w515p_eval.c
    )

target_include_directories(GD32W515P_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32W515P_EVAL
    )

target_link_libraries(GD32W515P_EVAL PUBLIC GD32W51x_F5HC_standard_peripheral)
