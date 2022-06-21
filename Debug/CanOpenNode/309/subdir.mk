################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CanOpenNode/309/CO_gateway_ascii.c 

C_DEPS += \
./CanOpenNode/309/CO_gateway_ascii.d 

OBJS += \
./CanOpenNode/309/CO_gateway_ascii.o 


# Each subdirectory must supply rules for building sources it contributes
CanOpenNode/309/%.o CanOpenNode/309/%.su: ../CanOpenNode/309/%.c CanOpenNode/309/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CanOpenNode-2f-309

clean-CanOpenNode-2f-309:
	-$(RM) ./CanOpenNode/309/CO_gateway_ascii.d ./CanOpenNode/309/CO_gateway_ascii.o ./CanOpenNode/309/CO_gateway_ascii.su

.PHONY: clean-CanOpenNode-2f-309

