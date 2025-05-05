################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/Target/main_s.c \
../CMSIS/Target/tz_context.c 

OBJS += \
./CMSIS/Target/main_s.o \
./CMSIS/Target/tz_context.o 

C_DEPS += \
./CMSIS/Target/main_s.d \
./CMSIS/Target/tz_context.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/Target/%.o CMSIS/Target/%.su CMSIS/Target/%.cyclo: ../CMSIS/Target/%.c CMSIS/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -DUSE_NUCLEO_64 -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/STM32H7xx_Nucleo -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/ARM_CMSIS/CMSIS/Core/Include/ -I../../Middlewares/Third_Party/ARM_CMSIS/CMSIS/Core/Include -I../../Middlewares/Third_Party/ARM_CMSIS/PrivateInclude/ -I../../Middlewares/Third_Party/ARM_CMSIS/Include/ -I../../Middlewares/Third_Party/ARM_CMSIS/Include -I../../Middlewares/Third_Party/ARM_CMSIS/ComputeLibrary/Include -I../../Packs/ARM/CMSIS-DSP/1.16.2/Include -I../../Packs/ARM/CMSIS-DSP/1.16.2/ComputeLibrary/Include -I../../Packs/ARM/CMSIS/6.2.0-dev3+gb7f79bb/CMSIS/Core/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CMSIS-2f-Target

clean-CMSIS-2f-Target:
	-$(RM) ./CMSIS/Target/main_s.cyclo ./CMSIS/Target/main_s.d ./CMSIS/Target/main_s.o ./CMSIS/Target/main_s.su ./CMSIS/Target/tz_context.cyclo ./CMSIS/Target/tz_context.d ./CMSIS/Target/tz_context.o ./CMSIS/Target/tz_context.su

.PHONY: clean-CMSIS-2f-Target

