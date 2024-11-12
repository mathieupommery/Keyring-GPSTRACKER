################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.c 

OBJS += \
./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.o 

C_DEPS += \
./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/NimaLTD_Driver/SPIF/%.o Middlewares/Third_Party/NimaLTD_Driver/SPIF/%.su Middlewares/Third_Party/NimaLTD_Driver/SPIF/%.cyclo: ../Middlewares/Third_Party/NimaLTD_Driver/SPIF/%.c Middlewares/Third_Party/NimaLTD_Driver/SPIF/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../I-CUBE-SPIF -I../STM32_WPAN/App -I../USB_Device/App -I../USB_Device/Target -I../Utilities/lpm/tiny_lpm -I../Middlewares/ST/STM32_WPAN -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../Middlewares/ST/STM32_WPAN/utilities -I../Middlewares/ST/STM32_WPAN/ble/core -I../Middlewares/ST/STM32_WPAN/ble/core/auto -I../Middlewares/ST/STM32_WPAN/ble/core/template -I../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Utilities/sequencer -I../Middlewares/ST/STM32_WPAN/ble -I../Middlewares/Third_Party/NimaLTD_Driver/SPIF -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-NimaLTD_Driver-2f-SPIF

clean-Middlewares-2f-Third_Party-2f-NimaLTD_Driver-2f-SPIF:
	-$(RM) ./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.cyclo ./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.d ./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.o ./Middlewares/Third_Party/NimaLTD_Driver/SPIF/spif.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-NimaLTD_Driver-2f-SPIF

