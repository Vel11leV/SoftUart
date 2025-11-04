################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../User_drivers/RingBuffer.cpp \
../User_drivers/SoftUart.cpp 

OBJS += \
./User_drivers/RingBuffer.o \
./User_drivers/SoftUart.o 

CPP_DEPS += \
./User_drivers/RingBuffer.d \
./User_drivers/SoftUart.d 


# Each subdirectory must supply rules for building sources it contributes
User_drivers/%.o: ../User_drivers/%.cpp User_drivers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mishc/Desktop/SoftUart/SoftwareUart/User_drivers" -I"C:/Users/mishc/Desktop/SoftUart/SoftwareUart/Libs/Header/Drivers" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_drivers

clean-User_drivers:
	-$(RM) ./User_drivers/RingBuffer.d ./User_drivers/RingBuffer.o ./User_drivers/SoftUart.d ./User_drivers/SoftUart.o

.PHONY: clean-User_drivers

