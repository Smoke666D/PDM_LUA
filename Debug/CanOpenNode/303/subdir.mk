################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CanOpenNode/303/CO_LEDs.c 

C_DEPS += \
./CanOpenNode/303/CO_LEDs.d 

OBJS += \
./CanOpenNode/303/CO_LEDs.o 


# Each subdirectory must supply rules for building sources it contributes
CanOpenNode/303/%.o CanOpenNode/303/%.su: ../CanOpenNode/303/%.c CanOpenNode/303/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CanOpenNode-2f-303

clean-CanOpenNode-2f-303:
	-$(RM) ./CanOpenNode/303/CO_LEDs.d ./CanOpenNode/303/CO_LEDs.o ./CanOpenNode/303/CO_LEDs.su

.PHONY: clean-CanOpenNode-2f-303

