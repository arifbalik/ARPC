################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/arpc_server/arpc_generic.c \
../Drivers/arpc_server/arpc_server.c 

OBJS += \
./Drivers/arpc_server/arpc_generic.o \
./Drivers/arpc_server/arpc_server.o 

C_DEPS += \
./Drivers/arpc_server/arpc_generic.d \
./Drivers/arpc_server/arpc_server.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/arpc_server/%.o Drivers/arpc_server/%.su Drivers/arpc_server/%.cyclo: ../Drivers/arpc_server/%.c Drivers/arpc_server/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/arpc_server -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-arpc_server

clean-Drivers-2f-arpc_server:
	-$(RM) ./Drivers/arpc_server/arpc_generic.cyclo ./Drivers/arpc_server/arpc_generic.d ./Drivers/arpc_server/arpc_generic.o ./Drivers/arpc_server/arpc_generic.su ./Drivers/arpc_server/arpc_server.cyclo ./Drivers/arpc_server/arpc_server.d ./Drivers/arpc_server/arpc_server.o ./Drivers/arpc_server/arpc_server.su

.PHONY: clean-Drivers-2f-arpc_server

