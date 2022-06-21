################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CanOpenNode/305/CO_LSSmaster.c \
../CanOpenNode/305/CO_LSSslave.c 

C_DEPS += \
./CanOpenNode/305/CO_LSSmaster.d \
./CanOpenNode/305/CO_LSSslave.d 

OBJS += \
./CanOpenNode/305/CO_LSSmaster.o \
./CanOpenNode/305/CO_LSSslave.o 


# Each subdirectory must supply rules for building sources it contributes
CanOpenNode/305/%.o CanOpenNode/305/%.su: ../CanOpenNode/305/%.c CanOpenNode/305/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../CanOpenNode/301 -I../CanOpenNode -I../CanOpenNode/extra -I../CanOpenNode/PDM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CanOpenNode-2f-305

clean-CanOpenNode-2f-305:
	-$(RM) ./CanOpenNode/305/CO_LSSmaster.d ./CanOpenNode/305/CO_LSSmaster.o ./CanOpenNode/305/CO_LSSmaster.su ./CanOpenNode/305/CO_LSSslave.d ./CanOpenNode/305/CO_LSSslave.o ./CanOpenNode/305/CO_LSSslave.su

.PHONY: clean-CanOpenNode-2f-305

