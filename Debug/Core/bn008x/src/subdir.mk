################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/bn008x/src/bn008x.c \
../Core/bn008x/src/bn008x_port.c 

OBJS += \
./Core/bn008x/src/bn008x.o \
./Core/bn008x/src/bn008x_port.o 

C_DEPS += \
./Core/bn008x/src/bn008x.d \
./Core/bn008x/src/bn008x_port.d 


# Each subdirectory must supply rules for building sources it contributes
Core/bn008x/src/%.o Core/bn008x/src/%.su Core/bn008x/src/%.cyclo: ../Core/bn008x/src/%.c Core/bn008x/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/lehas/OneDrive/Документы/Стартап/BN008X_Driver_STM32-main/Core/bn008x/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/Users/lehas/OneDrive/Документы/Стартап/BN008X_Driver_STM32-main/Core/bn008x" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-bn008x-2f-src

clean-Core-2f-bn008x-2f-src:
	-$(RM) ./Core/bn008x/src/bn008x.cyclo ./Core/bn008x/src/bn008x.d ./Core/bn008x/src/bn008x.o ./Core/bn008x/src/bn008x.su ./Core/bn008x/src/bn008x_port.cyclo ./Core/bn008x/src/bn008x_port.d ./Core/bn008x/src/bn008x_port.o ./Core/bn008x/src/bn008x_port.su

.PHONY: clean-Core-2f-bn008x-2f-src

