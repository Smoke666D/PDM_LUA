################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lua/lua/test/src/uart/trace_uart.c 

C_DEPS += \
./lua/lua/test/src/uart/trace_uart.d 

OBJS += \
./lua/lua/test/src/uart/trace_uart.o 


# Each subdirectory must supply rules for building sources it contributes
lua/lua/test/src/uart/%.o lua/lua/test/src/uart/%.su: ../lua/lua/test/src/uart/%.c lua/lua/test/src/uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lua-2f-lua-2f-test-2f-src-2f-uart

clean-lua-2f-lua-2f-test-2f-src-2f-uart:
	-$(RM) ./lua/lua/test/src/uart/trace_uart.d ./lua/lua/test/src/uart/trace_uart.o ./lua/lua/test/src/uart/trace_uart.su

.PHONY: clean-lua-2f-lua-2f-test-2f-src-2f-uart

