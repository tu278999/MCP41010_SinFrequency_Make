################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Mcp41010/src/adc.c \
../Mcp41010/src/mcp41010.c \
../Mcp41010/src/mcp_event_timer.c \
../Mcp41010/src/mcp_function.c \
../Mcp41010/src/mcphandledata.c \
../Mcp41010/src/uart_mcp.c 

OBJS += \
./Mcp41010/src/adc.o \
./Mcp41010/src/mcp41010.o \
./Mcp41010/src/mcp_event_timer.o \
./Mcp41010/src/mcp_function.o \
./Mcp41010/src/mcphandledata.o \
./Mcp41010/src/uart_mcp.o 

C_DEPS += \
./Mcp41010/src/adc.d \
./Mcp41010/src/mcp41010.d \
./Mcp41010/src/mcp_event_timer.d \
./Mcp41010/src/mcp_function.d \
./Mcp41010/src/mcphandledata.d \
./Mcp41010/src/uart_mcp.d 


# Each subdirectory must supply rules for building sources it contributes
Mcp41010/src/%.o: ../Mcp41010/src/%.c Mcp41010/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/Embedded-C/STM32/mcp41010_workspace2/MCP41010_SinFrequency_Make/Mcp41010/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

