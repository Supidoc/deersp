################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/aic3x.c \
../Core/Src/amp_m7.c \
../Core/Src/audio_buffer.c \
../Core/Src/audio_status.c \
../Core/Src/biquad_node.c \
../Core/Src/buffer_manager.c \
../Core/Src/dsp.c \
../Core/Src/graph_manager.c \
../Core/Src/input_node.c \
../Core/Src/main.c \
../Core/Src/mdma.c \
../Core/Src/mix_node.c \
../Core/Src/node_manager.c \
../Core/Src/output_node.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c 

OBJS += \
./Core/Src/aic3x.o \
./Core/Src/amp_m7.o \
./Core/Src/audio_buffer.o \
./Core/Src/audio_status.o \
./Core/Src/biquad_node.o \
./Core/Src/buffer_manager.o \
./Core/Src/dsp.o \
./Core/Src/graph_manager.o \
./Core/Src/input_node.o \
./Core/Src/main.o \
./Core/Src/mdma.o \
./Core/Src/mix_node.o \
./Core/Src/node_manager.o \
./Core/Src/output_node.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o 

C_DEPS += \
./Core/Src/aic3x.d \
./Core/Src/amp_m7.d \
./Core/Src/audio_buffer.d \
./Core/Src/audio_status.d \
./Core/Src/biquad_node.d \
./Core/Src/buffer_manager.d \
./Core/Src/dsp.d \
./Core/Src/graph_manager.d \
./Core/Src/input_node.d \
./Core/Src/main.d \
./Core/Src/mdma.d \
./Core/Src/mix_node.d \
./Core/Src/node_manager.d \
./Core/Src/output_node.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -DMETAL_INTERNAL -DMETAL_MAX_DEVICE_REGIONS=2 -DRPMSG_BUFFER_SIZE=512 -DVIRTIO_MASTER_ONLY -DNO_ATOMIC_64_SUPPORT -c -I../Core/Inc -I"D:/Documents/01_Projects/Deersp/deersp/Audio_Matrix/CM7/Drivers/CMSIS/DSP/Include" -I"D:/Documents/01_Projects/Deersp/deersp/Audio_Matrix/CM7/Drivers/CMSIS/DSP/PrivateInclude" -I../../Middlewares/Third_Party/ARM_CMSIS/CMSIS/Core/Include -I../../Middlewares/Third_Party/ARM_CMSIS/PrivateInclude -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../OPENAMP -I../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include -I../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include -I../../Middlewares/Third_Party/OpenAMP/libmetal/lib/include -I../../Middlewares/Third_Party/OpenAMP/libmetal/lib/include/metal/compiler/gcc -I../../Middlewares/Third_Party/OpenAMP/open-amp/lib/rpmsg -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/aic3x.cyclo ./Core/Src/aic3x.d ./Core/Src/aic3x.o ./Core/Src/aic3x.su ./Core/Src/amp_m7.cyclo ./Core/Src/amp_m7.d ./Core/Src/amp_m7.o ./Core/Src/amp_m7.su ./Core/Src/audio_buffer.cyclo ./Core/Src/audio_buffer.d ./Core/Src/audio_buffer.o ./Core/Src/audio_buffer.su ./Core/Src/audio_status.cyclo ./Core/Src/audio_status.d ./Core/Src/audio_status.o ./Core/Src/audio_status.su ./Core/Src/biquad_node.cyclo ./Core/Src/biquad_node.d ./Core/Src/biquad_node.o ./Core/Src/biquad_node.su ./Core/Src/buffer_manager.cyclo ./Core/Src/buffer_manager.d ./Core/Src/buffer_manager.o ./Core/Src/buffer_manager.su ./Core/Src/dsp.cyclo ./Core/Src/dsp.d ./Core/Src/dsp.o ./Core/Src/dsp.su ./Core/Src/graph_manager.cyclo ./Core/Src/graph_manager.d ./Core/Src/graph_manager.o ./Core/Src/graph_manager.su ./Core/Src/input_node.cyclo ./Core/Src/input_node.d ./Core/Src/input_node.o ./Core/Src/input_node.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mdma.cyclo ./Core/Src/mdma.d ./Core/Src/mdma.o ./Core/Src/mdma.su ./Core/Src/mix_node.cyclo ./Core/Src/mix_node.d ./Core/Src/mix_node.o ./Core/Src/mix_node.su ./Core/Src/node_manager.cyclo ./Core/Src/node_manager.d ./Core/Src/node_manager.o ./Core/Src/node_manager.su ./Core/Src/output_node.cyclo ./Core/Src/output_node.d ./Core/Src/output_node.o ./Core/Src/output_node.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su

.PHONY: clean-Core-2f-Src

