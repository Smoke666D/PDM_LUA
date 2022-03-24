################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lua/helper/luaHeap.c \
../lua/helper/luaHelper.c 

C_DEPS += \
./lua/helper/luaHeap.d \
./lua/helper/luaHelper.d 

OBJS += \
./lua/helper/luaHeap.o \
./lua/helper/luaHelper.o 


# Each subdirectory must supply rules for building sources it contributes
lua/helper/%.o lua/helper/%.su: ../lua/helper/%.c lua/helper/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lua-2f-helper

clean-lua-2f-helper:
	-$(RM) ./lua/helper/luaHeap.d ./lua/helper/luaHeap.o ./lua/helper/luaHeap.su ./lua/helper/luaHelper.d ./lua/helper/luaHelper.o ./lua/helper/luaHelper.su

.PHONY: clean-lua-2f-helper

