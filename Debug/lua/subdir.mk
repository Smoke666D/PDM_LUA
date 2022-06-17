################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LUA/lapi.c \
../LUA/lauxlib.c \
../LUA/lbaselib.c \
../LUA/lcode.c \
../LUA/lcorolib.c \
../LUA/lctype.c \
../LUA/ldblib.c \
../LUA/ldebug.c \
../LUA/ldo.c \
../LUA/ldump.c \
../LUA/lfunc.c \
../LUA/lgc.c \
../LUA/linit.c \
../LUA/liolib.c \
../LUA/llex.c \
../LUA/lmathlib.c \
../LUA/lmem.c \
../LUA/loadlib.c \
../LUA/lobject.c \
../LUA/lopcodes.c \
../LUA/loslib.c \
../LUA/lparser.c \
../LUA/lstate.c \
../LUA/lstring.c \
../LUA/lstrlib.c \
../LUA/ltable.c \
../LUA/ltablib.c \
../LUA/ltm.c \
../LUA/luaeheap.c \
../LUA/lundump.c \
../LUA/lutf8lib.c \
../LUA/lvm.c \
../LUA/lzio.c 

C_DEPS += \
./LUA/lapi.d \
./LUA/lauxlib.d \
./LUA/lbaselib.d \
./LUA/lcode.d \
./LUA/lcorolib.d \
./LUA/lctype.d \
./LUA/ldblib.d \
./LUA/ldebug.d \
./LUA/ldo.d \
./LUA/ldump.d \
./LUA/lfunc.d \
./LUA/lgc.d \
./LUA/linit.d \
./LUA/liolib.d \
./LUA/llex.d \
./LUA/lmathlib.d \
./LUA/lmem.d \
./LUA/loadlib.d \
./LUA/lobject.d \
./LUA/lopcodes.d \
./LUA/loslib.d \
./LUA/lparser.d \
./LUA/lstate.d \
./LUA/lstring.d \
./LUA/lstrlib.d \
./LUA/ltable.d \
./LUA/ltablib.d \
./LUA/ltm.d \
./LUA/luaeheap.d \
./LUA/lundump.d \
./LUA/lutf8lib.d \
./LUA/lvm.d \
./LUA/lzio.d 

OBJS += \
./LUA/lapi.o \
./LUA/lauxlib.o \
./LUA/lbaselib.o \
./LUA/lcode.o \
./LUA/lcorolib.o \
./LUA/lctype.o \
./LUA/ldblib.o \
./LUA/ldebug.o \
./LUA/ldo.o \
./LUA/ldump.o \
./LUA/lfunc.o \
./LUA/lgc.o \
./LUA/linit.o \
./LUA/liolib.o \
./LUA/llex.o \
./LUA/lmathlib.o \
./LUA/lmem.o \
./LUA/loadlib.o \
./LUA/lobject.o \
./LUA/lopcodes.o \
./LUA/loslib.o \
./LUA/lparser.o \
./LUA/lstate.o \
./LUA/lstring.o \
./LUA/lstrlib.o \
./LUA/ltable.o \
./LUA/ltablib.o \
./LUA/ltm.o \
./LUA/luaeheap.o \
./LUA/lundump.o \
./LUA/lutf8lib.o \
./LUA/lvm.o \
./LUA/lzio.o 


# Each subdirectory must supply rules for building sources it contributes
LUA/%.o LUA/%.su: ../LUA/%.c LUA/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../LUA -I../Shell -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LUA

clean-LUA:
	-$(RM) ./LUA/lapi.d ./LUA/lapi.o ./LUA/lapi.su ./LUA/lauxlib.d ./LUA/lauxlib.o ./LUA/lauxlib.su ./LUA/lbaselib.d ./LUA/lbaselib.o ./LUA/lbaselib.su ./LUA/lcode.d ./LUA/lcode.o ./LUA/lcode.su ./LUA/lcorolib.d ./LUA/lcorolib.o ./LUA/lcorolib.su ./LUA/lctype.d ./LUA/lctype.o ./LUA/lctype.su ./LUA/ldblib.d ./LUA/ldblib.o ./LUA/ldblib.su ./LUA/ldebug.d ./LUA/ldebug.o ./LUA/ldebug.su ./LUA/ldo.d ./LUA/ldo.o ./LUA/ldo.su ./LUA/ldump.d ./LUA/ldump.o ./LUA/ldump.su ./LUA/lfunc.d ./LUA/lfunc.o ./LUA/lfunc.su ./LUA/lgc.d ./LUA/lgc.o ./LUA/lgc.su ./LUA/linit.d ./LUA/linit.o ./LUA/linit.su ./LUA/liolib.d ./LUA/liolib.o ./LUA/liolib.su ./LUA/llex.d ./LUA/llex.o ./LUA/llex.su ./LUA/lmathlib.d ./LUA/lmathlib.o ./LUA/lmathlib.su ./LUA/lmem.d ./LUA/lmem.o ./LUA/lmem.su ./LUA/loadlib.d ./LUA/loadlib.o ./LUA/loadlib.su ./LUA/lobject.d ./LUA/lobject.o ./LUA/lobject.su ./LUA/lopcodes.d ./LUA/lopcodes.o ./LUA/lopcodes.su ./LUA/loslib.d ./LUA/loslib.o ./LUA/loslib.su ./LUA/lparser.d ./LUA/lparser.o ./LUA/lparser.su ./LUA/lstate.d ./LUA/lstate.o ./LUA/lstate.su ./LUA/lstring.d ./LUA/lstring.o ./LUA/lstring.su ./LUA/lstrlib.d ./LUA/lstrlib.o ./LUA/lstrlib.su ./LUA/ltable.d ./LUA/ltable.o ./LUA/ltable.su ./LUA/ltablib.d ./LUA/ltablib.o ./LUA/ltablib.su ./LUA/ltm.d ./LUA/ltm.o ./LUA/ltm.su ./LUA/luaeheap.d ./LUA/luaeheap.o ./LUA/luaeheap.su ./LUA/lundump.d ./LUA/lundump.o ./LUA/lundump.su ./LUA/lutf8lib.d ./LUA/lutf8lib.o ./LUA/lutf8lib.su ./LUA/lvm.d ./LUA/lvm.o ./LUA/lvm.su ./LUA/lzio.d ./LUA/lzio.o ./LUA/lzio.su

.PHONY: clean-LUA

