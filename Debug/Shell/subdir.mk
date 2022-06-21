################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Shell/PDMhardware.c \
../Shell/luatask.c \
../Shell/script.c 

C_DEPS += \
./Shell/PDMhardware.d \
./Shell/luatask.d \
./Shell/script.d 

OBJS += \
./Shell/PDMhardware.o \
./Shell/luatask.o \
./Shell/script.o 


# Each subdirectory must supply rules for building sources it contributes
Shell/%.o Shell/%.su: ../Shell/%.c Shell/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Shell

clean-Shell:
	-$(RM) ./Shell/PDMhardware.d ./Shell/PDMhardware.o ./Shell/PDMhardware.su ./Shell/luatask.d ./Shell/luatask.o ./Shell/luatask.su ./Shell/script.d ./Shell/script.o ./Shell/script.su

.PHONY: clean-Shell

