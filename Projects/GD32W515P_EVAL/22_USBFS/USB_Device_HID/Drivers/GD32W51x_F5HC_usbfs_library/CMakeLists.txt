project(GD32W51x_F5HC_usbfs_library LANGUAGES C CXX ASM)

add_library(GD32W51x_F5HC_usbfs_library OBJECT
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/class/hid/Source/standard_hid_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/core/Source/usbd_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/core/Source/usbd_enum.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/core/Source/usbd_transc.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usb_core.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usb_dev.c
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Source/drv_usbd_int.c
    )

target_include_directories(GD32W51x_F5HC_usbfs_library PUBLIC
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/core/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/class/hid/Include
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/ustd/class/hid
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/ustd/common
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/class/hid/Inc
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/device/core/Inc
    ${DRIVERS_DIR}/GD32W51x_F5HC_usbfs_library/driver/Inc
    ${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )

target_link_libraries(GD32W51x_F5HC_usbfs_library PUBLIC GD32W515P_EVAL)
