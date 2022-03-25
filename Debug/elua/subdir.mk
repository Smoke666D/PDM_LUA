################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../elua/lapi.c \
../elua/lauxlib.c \
../elua/lbaselib.c \
../elua/lcode.c \
../elua/lcorolib.c \
../elua/lctype.c \
../elua/ldblib.c \
../elua/ldebug.c \
../elua/ldo.c \
../elua/ldump.c \
../elua/lfunc.c \
../elua/lgc.c \
../elua/linit.c \
../elua/liolib.c \
../elua/llex.c \
../elua/lmathlib.c \
../elua/lmem.c \
../elua/loadlib.c \
../elua/lobject.c \
../elua/lopcodes.c \
../elua/loslib.c \
../elua/lparser.c \
../elua/lstate.c \
../elua/lstring.c \
../elua/lstrlib.c \
../elua/ltable.c \
../elua/ltablib.c \
../elua/ltm.c \
../elua/lua.c \
../elua/luac.c \
../elua/lundump.c \
../elua/lutf8lib.c \
../elua/lvm.c \
../elua/lzio.c 

C_DEPS += \
./elua/lapi.d \
./elua/lauxlib.d \
./elua/lbaselib.d \
./elua/lcode.d \
./elua/lcorolib.d \
./elua/lctype.d \
./elua/ldblib.d \
./elua/ldebug.d \
./elua/ldo.d \
./elua/ldump.d \
./elua/lfunc.d \
./elua/lgc.d \
./elua/linit.d \
./elua/liolib.d \
./elua/llex.d \
./elua/lmathlib.d \
./elua/lmem.d \
./elua/loadlib.d \
./elua/lobject.d \
./elua/lopcodes.d \
./elua/loslib.d \
./elua/lparser.d \
./elua/lstate.d \
./elua/lstring.d \
./elua/lstrlib.d \
./elua/ltable.d \
./elua/ltablib.d \
./elua/ltm.d \
./elua/lua.d \
./elua/luac.d \
./elua/lundump.d \
./elua/lutf8lib.d \
./elua/lvm.d \
./elua/lzio.d 

OBJS += \
./elua/lapi.o \
./elua/lauxlib.o \
./elua/lbaselib.o \
./elua/lcode.o \
./elua/lcorolib.o \
./elua/lctype.o \
./elua/ldblib.o \
./elua/ldebug.o \
./elua/ldo.o \
./elua/ldump.o \
./elua/lfunc.o \
./elua/lgc.o \
./elua/linit.o \
./elua/liolib.o \
./elua/llex.o \
./elua/lmathlib.o \
./elua/lmem.o \
./elua/loadlib.o \
./elua/lobject.o \
./elua/lopcodes.o \
./elua/loslib.o \
./elua/lparser.o \
./elua/lstate.o \
./elua/lstring.o \
./elua/lstrlib.o \
./elua/ltable.o \
./elua/ltablib.o \
./elua/ltm.o \
./elua/lua.o \
./elua/luac.o \
./elua/lundump.o \
./elua/lutf8lib.o \
./elua/lvm.o \
./elua/lzio.o 


# Each subdirectory must supply rules for building sources it contributes
elua/%.o elua/%.su: ../elua/%.c elua/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-elua

clean-elua:
	-$(RM) ./elua/lapi.d ./elua/lapi.o ./elua/lapi.su ./elua/lauxlib.d ./elua/lauxlib.o ./elua/lauxlib.su ./elua/lbaselib.d ./elua/lbaselib.o ./elua/lbaselib.su ./elua/lcode.d ./elua/lcode.o ./elua/lcode.su ./elua/lcorolib.d ./elua/lcorolib.o ./elua/lcorolib.su ./elua/lctype.d ./elua/lctype.o ./elua/lctype.su ./elua/ldblib.d ./elua/ldblib.o ./elua/ldblib.su ./elua/ldebug.d ./elua/ldebug.o ./elua/ldebug.su ./elua/ldo.d ./elua/ldo.o ./elua/ldo.su ./elua/ldump.d ./elua/ldump.o ./elua/ldump.su ./elua/lfunc.d ./elua/lfunc.o ./elua/lfunc.su ./elua/lgc.d ./elua/lgc.o ./elua/lgc.su ./elua/linit.d ./elua/linit.o ./elua/linit.su ./elua/liolib.d ./elua/liolib.o ./elua/liolib.su ./elua/llex.d ./elua/llex.o ./elua/llex.su ./elua/lmathlib.d ./elua/lmathlib.o ./elua/lmathlib.su ./elua/lmem.d ./elua/lmem.o ./elua/lmem.su ./elua/loadlib.d ./elua/loadlib.o ./elua/loadlib.su ./elua/lobject.d ./elua/lobject.o ./elua/lobject.su ./elua/lopcodes.d ./elua/lopcodes.o ./elua/lopcodes.su ./elua/loslib.d ./elua/loslib.o ./elua/loslib.su ./elua/lparser.d ./elua/lparser.o ./elua/lparser.su ./elua/lstate.d ./elua/lstate.o ./elua/lstate.su ./elua/lstring.d ./elua/lstring.o ./elua/lstring.su ./elua/lstrlib.d ./elua/lstrlib.o ./elua/lstrlib.su ./elua/ltable.d ./elua/ltable.o ./elua/ltable.su ./elua/ltablib.d ./elua/ltablib.o ./elua/ltablib.su ./elua/ltm.d ./elua/ltm.o ./elua/ltm.su ./elua/lua.d ./elua/lua.o ./elua/lua.su ./elua/luac.d ./elua/luac.o ./elua/luac.su ./elua/lundump.d ./elua/lundump.o ./elua/lundump.su ./elua/lutf8lib.d ./elua/lutf8lib.o ./elua/lutf8lib.su ./elua/lvm.d ./elua/lvm.o ./elua/lvm.su ./elua/lzio.d ./elua/lzio.o ./elua/lzio.su

.PHONY: clean-elua

