################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Shell/CO_driver_ST32F4xx.c \
../Shell/PDMhardware.c \
../Shell/cantask.c \
../Shell/luatask.c \
../Shell/pdm_lua_lib.c \
../Shell/script.c 

C_DEPS += \
./Shell/CO_driver_ST32F4xx.d \
./Shell/PDMhardware.d \
./Shell/cantask.d \
./Shell/luatask.d \
./Shell/pdm_lua_lib.d \
./Shell/script.d 

OBJS += \
./Shell/CO_driver_ST32F4xx.o \
./Shell/PDMhardware.o \
./Shell/cantask.o \
./Shell/luatask.o \
./Shell/pdm_lua_lib.o \
./Shell/script.o 


# Each subdirectory must supply rules for building sources it contributes
Shell/%.o Shell/%.su: ../Shell/%.c Shell/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Process -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Shell

clean-Shell:
	-$(RM) ./Shell/CO_driver_ST32F4xx.d ./Shell/CO_driver_ST32F4xx.o ./Shell/CO_driver_ST32F4xx.su ./Shell/PDMhardware.d ./Shell/PDMhardware.o ./Shell/PDMhardware.su ./Shell/cantask.d ./Shell/cantask.o ./Shell/cantask.su ./Shell/luatask.d ./Shell/luatask.o ./Shell/luatask.su ./Shell/pdm_lua_lib.d ./Shell/pdm_lua_lib.o ./Shell/pdm_lua_lib.su ./Shell/script.d ./Shell/script.o ./Shell/script.su

.PHONY: clean-Shell

