C_SOURCE_LIBS =\
Core/Src/main.c \
Core/Src/syscalls.c \
Core/Src/sysmem.c \
Core/Src/system_stm32f2xx.c \
Core/Src/stm32f2xx_hal_msp.c \
Core/Src/stm32f2xx_hal_timebase_tim.c \
Core/Src/stm32f2xx_it.c

C_USB_FILES = \
Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c  \
USB_DEVICE/Target/usbd_conf.c \
USB_DEVICE/App/usb_device.c \
USB_DEVICE/App/usbd_custom_hid_if.c \
USB_DEVICE/App/usbd_desc.c


C_FREERTOS_FILES = \
Core/Src/freertos.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c  \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c  \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c  \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c

C_FATFS_FILES = \
FATFS/Target/bsp_driver_sd.c \
FATFS/Target/fatfs_platform.c \
FATFS/App/fatfs.c \
Middlewares/Third_Party/FatFs/src/diskio.c \
Middlewares/Third_Party/FatFs/src/ff.c \
Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
Middlewares/Third_Party/FatFs/src/option/syscall.c \
Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c

C_LWIP_FILES = \
LWIP/Target/ethernetif.c \
LWIP/App/lwip.c \
Middlewares/Third_Party/LwIP/system/OS/sys_arch.c \
Middlewares/Third_Party/LwIP/src/netif/ethernet.c \
Middlewares/Third_Party/LwIP/src/netif/lowpan6.c \
Middlewares/Third_Party/LwIP/src/netif/slipif.c  \
Middlewares/Third_Party/LwIP/src/core/def.c \
Middlewares/Third_Party/LwIP/src/core/dns.c \
Middlewares/Third_Party/LwIP/src/core/inet_chksum.c \
Middlewares/Third_Party/LwIP/src/core/init.c \
Middlewares/Third_Party/LwIP/src/core/ip.c \
Middlewares/Third_Party/LwIP/src/core/mem.c \
Middlewares/Third_Party/LwIP/src/core/memp.c \
Middlewares/Third_Party/LwIP/src/core/netif.c \
Middlewares/Third_Party/LwIP/src/core/pbuf.c \
Middlewares/Third_Party/LwIP/src/core/raw.c \
Middlewares/Third_Party/LwIP/src/core/stats.c \
Middlewares/Third_Party/LwIP/src/core/sys.c \
Middlewares/Third_Party/LwIP/src/core/tcp.c \
Middlewares/Third_Party/LwIP/src/core/tcp_in.c \
Middlewares/Third_Party/LwIP/src/core/tcp_out.c \
Middlewares/Third_Party/LwIP/src/core/timeouts.c \
Middlewares/Third_Party/LwIP/src/core/udp.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c \
Middlewares/Third_Party/LwIP/src/apps/mqtt/mqtt.c \
Middlewares/Third_Party/LwIP/src/api/api_lib.c \
Middlewares/Third_Party/LwIP/src/api/api_msg.c \
Middlewares/Third_Party/LwIP/src/api/err.c \
Middlewares/Third_Party/LwIP/src/api/netbuf.c \
Middlewares/Third_Party/LwIP/src/api/netdb.c \
Middlewares/Third_Party/LwIP/src/api/netifapi.c \
Middlewares/Third_Party/LwIP/src/api/sockets.c \
Middlewares/Third_Party/LwIP/src/api/tcpip.c

C_LIBFIXMATH_FILES = \
libfixmath/Src/fix16.c \
libfixmath/Src/fix16_exp.c \
libfixmath/Src/fix16_sqrt.c \
libfixmath/Src/fix16_str.c \
libfixmath/Src/fix16_trig.c \
libfixmath/Src/fract32.c \
libfixmath/Src/uint32.c \

C_HAL_FILES = \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_adc.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_adc_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_cortex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_dma.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_dma_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_eth.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_exti.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_flash.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_flash_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_gpio.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_i2c.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_iwdg.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_pcd.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_pcd_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_pwr.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_pwr_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_rcc.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_rcc_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_sd.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_spi.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_tim.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_tim_ex.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_uart.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_ll_sdmmc.c \
Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_ll_usb.c

C_U8G2_FILES = \
u8g2/Src/u8g2_box.c \
u8g2/Src/u8g2_buffer.c \
u8g2/Src/u8g2_font.c \
u8g2/Src/u8g2_fonts.c \
u8g2/Src/u8g2_hvline.c \
u8g2/Src/u8g2_ll_hvline.c \
u8g2/Src/u8g2_line.c \
u8g2/Src/u8g2_d_setup.c \
u8g2/Src/u8g2_setup.c \
u8g2/Src/u8g2_d_memory.c \
u8g2/Src/u8x8_setup.c \
u8g2/Src/u8x8_8x8.c \
u8g2/Src/u8x8_cad.c \
u8g2/Src/u8x8_display.c \
u8g2/Src/u8x8_gpio.c \
u8g2/Src/u8x8_byte.c \
u8g2/Src/u8x8_d_st7920.c \
u8g2/Src/u8g2_intersection.c

C_SOURCES_TEST = \
Unity/unity.c \
Unity/unity_config.c \
Tests/Src/test_chart.c \
Tests/Src/test_cli.c \
Tests/Src/test_dataAPI.c \
Tests/Src/test_eeprom.c \
Tests/Src/test_http.c \
Tests/Src/test_outputProcessing.c \
Tests/Src/test_rest.c \
Tests/Src/test_rtc.c \
Tests/Src/test_storage.c \
Tests/Src/test_usbhid.c \
Tests/Src/test_vr.c \
Tests/Src/tests.c

C_SOURCE_PROJ =\
ModBus/Src/mb.c \
ModBus/Src/mbcrc.c \
ModBus/Src/mbfunc.c \
ModBus/Src/mbregister.c \
ModBus/Src/mbrtu.c \
ModBus/Src/mbuart.c \
hmi/Src/data_manager.c \
hmi/Src/keyboard.c \
hmi/Src/lcd.c \
hmi/Src/menu.c \
hmi/Src/menu_data.c \
hmi/Src/utils.c \
ethernet/Src/http.c \
ethernet/Src/rest.c \
ethernet/Src/server.c \
dsp/Src/pid.c \
devices/Src/EEPROM.c \
devices/Src/RTC.c \
devices/Src/adc.c \
devices/Src/fatsd.c \
devices/Src/mac.c \
devices/Src/usbhid.c \
data/Src/chart.c \
data/Src/config.c \
data/Src/dataAPI.c \
data/Src/dataSD.c \
data/Src/freeData.c \
data/Src/outputData.c \
data/Src/outputProcessing.c \
data/Src/storage.c \
controller/Src/alarm.c \
controller/Src/charger.c \
controller/Src/controller.c \
controller/Src/controllerTypes.c \
controller/Src/dataProces.c \
controller/Src/electro.c \
controller/Src/engine.c \
controller/Src/fpi.c \
controller/Src/fpo.c \
controller/Src/journal.c \
controller/Src/measurement.c \
controller/Src/statistics.c \
controller/Src/vrSensor.c \
common/Src/cli.c \
common/Src/common.c \
common/Src/system.c \
common/Src/constants.c

C_SOURCES      = $(C_HAL_FILES) $(C_U8G2_FILES) $(C_LIBFIXMATH_FILES) $(C_LWIP_FILES) $(C_FATFS_FILES) $(C_FREERTOS_FILES) $(C_USB_FILES) $(C_SOURCE_LIBS) $(C_SOURCE_PROJ)
C_TEST_SOURCES = $(C_SOURCES) $(C_SOURCES_TEST)