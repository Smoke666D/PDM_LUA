################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lua/lapi.c \
../lua/lauxlib.c \
../lua/lbaselib.c \
../lua/lcode.c \
../lua/lctype.c \
../lua/ldebug.c \
../lua/ldo.c \
../lua/lfunc.c \
../lua/lgc.c \
../lua/linit.c \
../lua/llex.c \
../lua/lmathlib.c \
../lua/lmem.c \
../lua/lobject.c \
../lua/lopcodes.c \
../lua/lparser.c \
../lua/lstate.c \
../lua/lstring.c \
../lua/lstrlib.c \
../lua/ltable.c \
../lua/ltablib.c \
../lua/ltm.c \
../lua/luaInterface.c \
../lua/lundump.c \
../lua/lvm.c \
../lua/lzio.c 

C_DEPS += \
./lua/lapi.d \
./lua/lauxlib.d \
./lua/lbaselib.d \
./lua/lcode.d \
./lua/lctype.d \
./lua/ldebug.d \
./lua/ldo.d \
./lua/lfunc.d \
./lua/lgc.d \
./lua/linit.d \
./lua/llex.d \
./lua/lmathlib.d \
./lua/lmem.d \
./lua/lobject.d \
./lua/lopcodes.d \
./lua/lparser.d \
./lua/lstate.d \
./lua/lstring.d \
./lua/lstrlib.d \
./lua/ltable.d \
./lua/ltablib.d \
./lua/ltm.d \
./lua/luaInterface.d \
./lua/lundump.d \
./lua/lvm.d \
./lua/lzio.d 

OBJS += \
./lua/lapi.o \
./lua/lauxlib.o \
./lua/lbaselib.o \
./lua/lcode.o \
./lua/lctype.o \
./lua/ldebug.o \
./lua/ldo.o \
./lua/lfunc.o \
./lua/lgc.o \
./lua/linit.o \
./lua/llex.o \
./lua/lmathlib.o \
./lua/lmem.o \
./lua/lobject.o \
./lua/lopcodes.o \
./lua/lparser.o \
./lua/lstate.o \
./lua/lstring.o \
./lua/lstrlib.o \
./lua/ltable.o \
./lua/ltablib.o \
./lua/ltm.o \
./lua/luaInterface.o \
./lua/lundump.o \
./lua/lvm.o \
./lua/lzio.o 


# Each subdirectory must supply rules for building sources it contributes
lua/%.o lua/%.su: ../lua/%.c lua/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lua

clean-lua:
	-$(RM) ./lua/lapi.d ./lua/lapi.o ./lua/lapi.su ./lua/lauxlib.d ./lua/lauxlib.o ./lua/lauxlib.su ./lua/lbaselib.d ./lua/lbaselib.o ./lua/lbaselib.su ./lua/lcode.d ./lua/lcode.o ./lua/lcode.su ./lua/lctype.d ./lua/lctype.o ./lua/lctype.su ./lua/ldebug.d ./lua/ldebug.o ./lua/ldebug.su ./lua/ldo.d ./lua/ldo.o ./lua/ldo.su ./lua/lfunc.d ./lua/lfunc.o ./lua/lfunc.su ./lua/lgc.d ./lua/lgc.o ./lua/lgc.su ./lua/linit.d ./lua/linit.o ./lua/linit.su ./lua/llex.d ./lua/llex.o ./lua/llex.su ./lua/lmathlib.d ./lua/lmathlib.o ./lua/lmathlib.su ./lua/lmem.d ./lua/lmem.o ./lua/lmem.su ./lua/lobject.d ./lua/lobject.o ./lua/lobject.su ./lua/lopcodes.d ./lua/lopcodes.o ./lua/lopcodes.su ./lua/lparser.d ./lua/lparser.o ./lua/lparser.su ./lua/lstate.d ./lua/lstate.o ./lua/lstate.su ./lua/lstring.d ./lua/lstring.o ./lua/lstring.su ./lua/lstrlib.d ./lua/lstrlib.o ./lua/lstrlib.su ./lua/ltable.d ./lua/ltable.o ./lua/ltable.su ./lua/ltablib.d ./lua/ltablib.o ./lua/ltablib.su ./lua/ltm.d ./lua/ltm.o ./lua/ltm.su ./lua/luaInterface.d ./lua/luaInterface.o ./lua/luaInterface.su ./lua/lundump.d ./lua/lundump.o ./lua/lundump.su ./lua/lvm.d ./lua/lvm.o ./lua/lvm.su ./lua/lzio.d ./lua/lzio.o ./lua/lzio.su

.PHONY: clean-lua

