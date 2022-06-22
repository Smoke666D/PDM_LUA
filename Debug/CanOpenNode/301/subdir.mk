################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CanOpenNode/301/CO_Emergency.c \
../CanOpenNode/301/CO_HBconsumer.c \
../CanOpenNode/301/CO_NMT_Heartbeat.c \
../CanOpenNode/301/CO_ODinterface.c \
../CanOpenNode/301/CO_PDO.c \
../CanOpenNode/301/CO_SDOclient.c \
../CanOpenNode/301/CO_SDOserver.c \
../CanOpenNode/301/CO_SYNC.c \
../CanOpenNode/301/CO_TIME.c \
../CanOpenNode/301/CO_fifo.c \
../CanOpenNode/301/crc16-ccitt.c 

C_DEPS += \
./CanOpenNode/301/CO_Emergency.d \
./CanOpenNode/301/CO_HBconsumer.d \
./CanOpenNode/301/CO_NMT_Heartbeat.d \
./CanOpenNode/301/CO_ODinterface.d \
./CanOpenNode/301/CO_PDO.d \
./CanOpenNode/301/CO_SDOclient.d \
./CanOpenNode/301/CO_SDOserver.d \
./CanOpenNode/301/CO_SYNC.d \
./CanOpenNode/301/CO_TIME.d \
./CanOpenNode/301/CO_fifo.d \
./CanOpenNode/301/crc16-ccitt.d 

OBJS += \
./CanOpenNode/301/CO_Emergency.o \
./CanOpenNode/301/CO_HBconsumer.o \
./CanOpenNode/301/CO_NMT_Heartbeat.o \
./CanOpenNode/301/CO_ODinterface.o \
./CanOpenNode/301/CO_PDO.o \
./CanOpenNode/301/CO_SDOclient.o \
./CanOpenNode/301/CO_SDOserver.o \
./CanOpenNode/301/CO_SYNC.o \
./CanOpenNode/301/CO_TIME.o \
./CanOpenNode/301/CO_fifo.o \
./CanOpenNode/301/crc16-ccitt.o 


# Each subdirectory must supply rules for building sources it contributes
CanOpenNode/301/%.o CanOpenNode/301/%.su: ../CanOpenNode/301/%.c CanOpenNode/301/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -I../Process -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CanOpenNode-2f-301

clean-CanOpenNode-2f-301:
	-$(RM) ./CanOpenNode/301/CO_Emergency.d ./CanOpenNode/301/CO_Emergency.o ./CanOpenNode/301/CO_Emergency.su ./CanOpenNode/301/CO_HBconsumer.d ./CanOpenNode/301/CO_HBconsumer.o ./CanOpenNode/301/CO_HBconsumer.su ./CanOpenNode/301/CO_NMT_Heartbeat.d ./CanOpenNode/301/CO_NMT_Heartbeat.o ./CanOpenNode/301/CO_NMT_Heartbeat.su ./CanOpenNode/301/CO_ODinterface.d ./CanOpenNode/301/CO_ODinterface.o ./CanOpenNode/301/CO_ODinterface.su ./CanOpenNode/301/CO_PDO.d ./CanOpenNode/301/CO_PDO.o ./CanOpenNode/301/CO_PDO.su ./CanOpenNode/301/CO_SDOclient.d ./CanOpenNode/301/CO_SDOclient.o ./CanOpenNode/301/CO_SDOclient.su ./CanOpenNode/301/CO_SDOserver.d ./CanOpenNode/301/CO_SDOserver.o ./CanOpenNode/301/CO_SDOserver.su ./CanOpenNode/301/CO_SYNC.d ./CanOpenNode/301/CO_SYNC.o ./CanOpenNode/301/CO_SYNC.su ./CanOpenNode/301/CO_TIME.d ./CanOpenNode/301/CO_TIME.o ./CanOpenNode/301/CO_TIME.su ./CanOpenNode/301/CO_fifo.d ./CanOpenNode/301/CO_fifo.o ./CanOpenNode/301/CO_fifo.su ./CanOpenNode/301/crc16-ccitt.d ./CanOpenNode/301/crc16-ccitt.o ./CanOpenNode/301/crc16-ccitt.su

.PHONY: clean-CanOpenNode-2f-301

