################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/bn008x/test/bn008x_hal_mock.c \
../Core/bn008x/test/test_bn008x.c 

OBJS += \
./Core/bn008x/test/bn008x_hal_mock.o \
./Core/bn008x/test/test_bn008x.o 

C_DEPS += \
./Core/bn008x/test/bn008x_hal_mock.d \
./Core/bn008x/test/test_bn008x.d 


# Each subdirectory must supply rules for building sources it contributes
Core/bn008x/test/%.o Core/bn008x/test/%.su Core/bn008x/test/%.cyclo: ../Core/bn008x/test/%.c Core/bn008x/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/lehas/OneDrive/Документы/Стартап/bn008x/Core/bn008x/test" -I"C:/Users/lehas/OneDrive/Документы/Стартап/bn008x/Core/bn008x" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-bn008x-2f-test

clean-Core-2f-bn008x-2f-test:
	-$(RM) ./Core/bn008x/test/bn008x_hal_mock.cyclo ./Core/bn008x/test/bn008x_hal_mock.d ./Core/bn008x/test/bn008x_hal_mock.o ./Core/bn008x/test/bn008x_hal_mock.su ./Core/bn008x/test/test_bn008x.cyclo ./Core/bn008x/test/test_bn008x.d ./Core/bn008x/test/test_bn008x.o ./Core/bn008x/test/test_bn008x.su

.PHONY: clean-Core-2f-bn008x-2f-test

