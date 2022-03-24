################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lua/lua/lapi.c \
../lua/lua/lauxlib.c \
../lua/lua/lbaselib.c \
../lua/lua/lcode.c \
../lua/lua/lctype.c \
../lua/lua/ldebug.c \
../lua/lua/ldo.c \
../lua/lua/lfunc.c \
../lua/lua/lgc.c \
../lua/lua/linit.c \
../lua/lua/llex.c \
../lua/lua/lmathlib.c \
../lua/lua/lmem.c \
../lua/lua/lobject.c \
../lua/lua/lopcodes.c \
../lua/lua/lparser.c \
../lua/lua/lstate.c \
../lua/lua/lstring.c \
../lua/lua/lstrlib.c \
../lua/lua/ltable.c \
../lua/lua/ltablib.c \
../lua/lua/ltm.c \
../lua/lua/lundump.c \
../lua/lua/lvm.c \
../lua/lua/lzio.c 

C_DEPS += \
./lua/lua/lapi.d \
./lua/lua/lauxlib.d \
./lua/lua/lbaselib.d \
./lua/lua/lcode.d \
./lua/lua/lctype.d \
./lua/lua/ldebug.d \
./lua/lua/ldo.d \
./lua/lua/lfunc.d \
./lua/lua/lgc.d \
./lua/lua/linit.d \
./lua/lua/llex.d \
./lua/lua/lmathlib.d \
./lua/lua/lmem.d \
./lua/lua/lobject.d \
./lua/lua/lopcodes.d \
./lua/lua/lparser.d \
./lua/lua/lstate.d \
./lua/lua/lstring.d \
./lua/lua/lstrlib.d \
./lua/lua/ltable.d \
./lua/lua/ltablib.d \
./lua/lua/ltm.d \
./lua/lua/lundump.d \
./lua/lua/lvm.d \
./lua/lua/lzio.d 

OBJS += \
./lua/lua/lapi.o \
./lua/lua/lauxlib.o \
./lua/lua/lbaselib.o \
./lua/lua/lcode.o \
./lua/lua/lctype.o \
./lua/lua/ldebug.o \
./lua/lua/ldo.o \
./lua/lua/lfunc.o \
./lua/lua/lgc.o \
./lua/lua/linit.o \
./lua/lua/llex.o \
./lua/lua/lmathlib.o \
./lua/lua/lmem.o \
./lua/lua/lobject.o \
./lua/lua/lopcodes.o \
./lua/lua/lparser.o \
./lua/lua/lstate.o \
./lua/lua/lstring.o \
./lua/lua/lstrlib.o \
./lua/lua/ltable.o \
./lua/lua/ltablib.o \
./lua/lua/ltm.o \
./lua/lua/lundump.o \
./lua/lua/lvm.o \
./lua/lua/lzio.o 


# Each subdirectory must supply rules for building sources it contributes
lua/lua/%.o lua/lua/%.su: ../lua/lua/%.c lua/lua/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lua-2f-lua

clean-lua-2f-lua:
	-$(RM) ./lua/lua/lapi.d ./lua/lua/lapi.o ./lua/lua/lapi.su ./lua/lua/lauxlib.d ./lua/lua/lauxlib.o ./lua/lua/lauxlib.su ./lua/lua/lbaselib.d ./lua/lua/lbaselib.o ./lua/lua/lbaselib.su ./lua/lua/lcode.d ./lua/lua/lcode.o ./lua/lua/lcode.su ./lua/lua/lctype.d ./lua/lua/lctype.o ./lua/lua/lctype.su ./lua/lua/ldebug.d ./lua/lua/ldebug.o ./lua/lua/ldebug.su ./lua/lua/ldo.d ./lua/lua/ldo.o ./lua/lua/ldo.su ./lua/lua/lfunc.d ./lua/lua/lfunc.o ./lua/lua/lfunc.su ./lua/lua/lgc.d ./lua/lua/lgc.o ./lua/lua/lgc.su ./lua/lua/linit.d ./lua/lua/linit.o ./lua/lua/linit.su ./lua/lua/llex.d ./lua/lua/llex.o ./lua/lua/llex.su ./lua/lua/lmathlib.d ./lua/lua/lmathlib.o ./lua/lua/lmathlib.su ./lua/lua/lmem.d ./lua/lua/lmem.o ./lua/lua/lmem.su ./lua/lua/lobject.d ./lua/lua/lobject.o ./lua/lua/lobject.su ./lua/lua/lopcodes.d ./lua/lua/lopcodes.o ./lua/lua/lopcodes.su ./lua/lua/lparser.d ./lua/lua/lparser.o ./lua/lua/lparser.su ./lua/lua/lstate.d ./lua/lua/lstate.o ./lua/lua/lstate.su ./lua/lua/lstring.d ./lua/lua/lstring.o ./lua/lua/lstring.su ./lua/lua/lstrlib.d ./lua/lua/lstrlib.o ./lua/lua/lstrlib.su ./lua/lua/ltable.d ./lua/lua/ltable.o ./lua/lua/ltable.su ./lua/lua/ltablib.d ./lua/lua/ltablib.o ./lua/lua/ltablib.su ./lua/lua/ltm.d ./lua/lua/ltm.o ./lua/lua/ltm.su ./lua/lua/lundump.d ./lua/lua/lundump.o ./lua/lua/lundump.su ./lua/lua/lvm.d ./lua/lua/lvm.o ./lua/lua/lvm.su ./lua/lua/lzio.d ./lua/lua/lzio.o ./lua/lua/lzio.su

.PHONY: clean-lua-2f-lua

