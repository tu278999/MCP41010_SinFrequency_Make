################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus/port/e_port.c \
../modbus/port/portevent.c \
../modbus/port/portserial.c \
../modbus/port/porttimer.c \
../modbus/port/user_mb_app.c 

OBJS += \
./modbus/port/e_port.o \
./modbus/port/portevent.o \
./modbus/port/portserial.o \
./modbus/port/porttimer.o \
./modbus/port/user_mb_app.o 

C_DEPS += \
./modbus/port/e_port.d \
./modbus/port/portevent.d \
./modbus/port/portserial.d \
./modbus/port/porttimer.d \
./modbus/port/user_mb_app.d 


# Each subdirectory must supply rules for building sources it contributes
modbus/port/%.o: ../modbus/port/%.c modbus/port/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/Embedded-C/STM32/mcpANDmodbus/test/modbus/include" -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/Embedded-C/STM32/mcpANDmodbus/test/Mcp41010_rs485/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

