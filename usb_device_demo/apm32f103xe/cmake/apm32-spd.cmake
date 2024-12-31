# APM32 SPD core sources
file(GLOB_RECURSE APM32_SPD_CORE_SOURCES
    "application/*.*"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_dma.c"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_misc.c"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_rcm.c"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_gpio.c"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_pmu.c"
    "driver/APM32F10x_StdPeriphDriver/src/apm32f10x_usb.c"
    "driver/Device/Geehy/APM32F10x/Source/gcc/startup_apm32f10x_hd.S"
)

# APM32 SPD core includes
set(APM32_SPD_CORE_INCLUDES
    "application/include"
    "application/config/include"
    "driver/APM32F10x_StdPeriphDriver/inc"
    "driver/Device/Geehy/APM32F10x/Include"
    "driver/CMSIS/Include"
)

# APM32 SPD drivers defines
set(APM32_SPD_CORE_DEFINES
    "APM32F10X_HD"
    "USB_DEVICE"
)