################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/User/Src/user_tca9548a.c 

OBJS += \
./Drivers/User/Src/user_tca9548a.o 

C_DEPS += \
./Drivers/User/Src/user_tca9548a.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/User/Src/%.o Drivers/User/Src/%.su Drivers/User/Src/%.cyclo: ../Drivers/User/Src/%.c Drivers/User/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -DUSE_NUCLEO_64 -c -I../Core/Inc -I../Drivers/User/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/STM32H7xx_Nucleo -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-User-2f-Src

clean-Drivers-2f-User-2f-Src:
	-$(RM) ./Drivers/User/Src/user_tca9548a.cyclo ./Drivers/User/Src/user_tca9548a.d ./Drivers/User/Src/user_tca9548a.o ./Drivers/User/Src/user_tca9548a.su

.PHONY: clean-Drivers-2f-User-2f-Src

