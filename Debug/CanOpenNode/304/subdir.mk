################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CanOpenNode/304/CO_GFC.c \
../CanOpenNode/304/CO_SRDO.c 

C_DEPS += \
./CanOpenNode/304/CO_GFC.d \
./CanOpenNode/304/CO_SRDO.d 

OBJS += \
./CanOpenNode/304/CO_GFC.o \
./CanOpenNode/304/CO_SRDO.o 


# Each subdirectory must supply rules for building sources it contributes
CanOpenNode/304/%.o CanOpenNode/304/%.su: ../CanOpenNode/304/%.c CanOpenNode/304/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CanOpenNode-2f-304

clean-CanOpenNode-2f-304:
	-$(RM) ./CanOpenNode/304/CO_GFC.d ./CanOpenNode/304/CO_GFC.o ./CanOpenNode/304/CO_GFC.su ./CanOpenNode/304/CO_SRDO.d ./CanOpenNode/304/CO_SRDO.o ./CanOpenNode/304/CO_SRDO.su

.PHONY: clean-CanOpenNode-2f-304

