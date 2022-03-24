################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lua/lua/test/src/dbsct.c \
../lua/lua/test/src/interrupt_handlers.c \
../lua/lua/test/src/lowsrc.c \
../lua/lua/test/src/r_init_clock.c \
../lua/lua/test/src/r_init_stop_module.c \
../lua/lua/test/src/reset_program.c \
../lua/lua/test/src/sbrk.c \
../lua/lua/test/src/vector_table.c 

CPP_SRCS += \
../lua/lua/test/src/main.cpp 

C_DEPS += \
./lua/lua/test/src/dbsct.d \
./lua/lua/test/src/interrupt_handlers.d \
./lua/lua/test/src/lowsrc.d \
./lua/lua/test/src/r_init_clock.d \
./lua/lua/test/src/r_init_stop_module.d \
./lua/lua/test/src/reset_program.d \
./lua/lua/test/src/sbrk.d \
./lua/lua/test/src/vector_table.d 

OBJS += \
./lua/lua/test/src/dbsct.o \
./lua/lua/test/src/interrupt_handlers.o \
./lua/lua/test/src/lowsrc.o \
./lua/lua/test/src/main.o \
./lua/lua/test/src/r_init_clock.o \
./lua/lua/test/src/r_init_stop_module.o \
./lua/lua/test/src/reset_program.o \
./lua/lua/test/src/sbrk.o \
./lua/lua/test/src/vector_table.o 

CPP_DEPS += \
./lua/lua/test/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
lua/lua/test/src/%.o lua/lua/test/src/%.su: ../lua/lua/test/src/%.c lua/lua/test/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
lua/lua/test/src/%.o lua/lua/test/src/%.su: ../lua/lua/test/src/%.cpp lua/lua/test/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lua-2f-lua-2f-test-2f-src

clean-lua-2f-lua-2f-test-2f-src:
	-$(RM) ./lua/lua/test/src/dbsct.d ./lua/lua/test/src/dbsct.o ./lua/lua/test/src/dbsct.su ./lua/lua/test/src/interrupt_handlers.d ./lua/lua/test/src/interrupt_handlers.o ./lua/lua/test/src/interrupt_handlers.su ./lua/lua/test/src/lowsrc.d ./lua/lua/test/src/lowsrc.o ./lua/lua/test/src/lowsrc.su ./lua/lua/test/src/main.d ./lua/lua/test/src/main.o ./lua/lua/test/src/main.su ./lua/lua/test/src/r_init_clock.d ./lua/lua/test/src/r_init_clock.o ./lua/lua/test/src/r_init_clock.su ./lua/lua/test/src/r_init_stop_module.d ./lua/lua/test/src/r_init_stop_module.o ./lua/lua/test/src/r_init_stop_module.su ./lua/lua/test/src/reset_program.d ./lua/lua/test/src/reset_program.o ./lua/lua/test/src/reset_program.su ./lua/lua/test/src/sbrk.d ./lua/lua/test/src/sbrk.o ./lua/lua/test/src/sbrk.su ./lua/lua/test/src/vector_table.d ./lua/lua/test/src/vector_table.o ./lua/lua/test/src/vector_table.su

.PHONY: clean-lua-2f-lua-2f-test-2f-src

