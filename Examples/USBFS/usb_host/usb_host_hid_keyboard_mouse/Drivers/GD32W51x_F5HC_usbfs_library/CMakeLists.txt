project(GD32W51x_F5HC_usbfs_library LANGUAGES C CXX ASM)

add_library(GD32W51x_F5HC_usbfs_library OBJECT
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usb_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usb_host.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usbh_int.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/class/hid/Source/usbh_hid_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/class/hid/Source/usbh_standard_hid.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Source/usbh_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Source/usbh_enum.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Source/usbh_pipe.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Source/usbh_transc.c
    )

target_include_directories(GD32W51x_F5HC_usbfs_library PUBLIC
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/class/hid/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/ustd/class/hid
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/ustd/common
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Inc
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/class/hid/Inc
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/host/core/Inc
    ${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )

target_link_libraries(GD32W51x_F5HC_usbfs_library PUBLIC GD32W515P_EVAL)
