################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/bmp581.c \
../Core/Src/dma.c \
../Core/Src/filter.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/ipcc.c \
../Core/Src/main.c \
../Core/Src/memorymap.c \
../Core/Src/nmea_parse.c \
../Core/Src/pulse_oximeter.c \
../Core/Src/rtc.c \
../Core/Src/spi.c \
../Core/Src/ssd1306.c \
../Core/Src/ssd1306_fonts.c \
../Core/Src/statemachine.c \
../Core/Src/stm32wbxx_hal_msp.c \
../Core/Src/stm32wbxx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32wbxx.c \
../Core/Src/tarvos.c \
../Core/Src/tim.c \
../Core/Src/usart.c \
../Core/Src/usb.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/bmp581.o \
./Core/Src/dma.o \
./Core/Src/filter.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/ipcc.o \
./Core/Src/main.o \
./Core/Src/memorymap.o \
./Core/Src/nmea_parse.o \
./Core/Src/pulse_oximeter.o \
./Core/Src/rtc.o \
./Core/Src/spi.o \
./Core/Src/ssd1306.o \
./Core/Src/ssd1306_fonts.o \
./Core/Src/statemachine.o \
./Core/Src/stm32wbxx_hal_msp.o \
./Core/Src/stm32wbxx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32wbxx.o \
./Core/Src/tarvos.o \
./Core/Src/tim.o \
./Core/Src/usart.o \
./Core/Src/usb.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/bmp581.d \
./Core/Src/dma.d \
./Core/Src/filter.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/ipcc.d \
./Core/Src/main.d \
./Core/Src/memorymap.d \
./Core/Src/nmea_parse.d \
./Core/Src/pulse_oximeter.d \
./Core/Src/rtc.d \
./Core/Src/spi.d \
./Core/Src/ssd1306.d \
./Core/Src/ssd1306_fonts.d \
./Core/Src/statemachine.d \
./Core/Src/stm32wbxx_hal_msp.d \
./Core/Src/stm32wbxx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32wbxx.d \
./Core/Src/tarvos.d \
./Core/Src/tim.d \
./Core/Src/usart.d \
./Core/Src/usb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../I-CUBE-SPIF -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/NimaLTD_Driver/SPIF -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/bmp581.cyclo ./Core/Src/bmp581.d ./Core/Src/bmp581.o ./Core/Src/bmp581.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/filter.cyclo ./Core/Src/filter.d ./Core/Src/filter.o ./Core/Src/filter.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/ipcc.cyclo ./Core/Src/ipcc.d ./Core/Src/ipcc.o ./Core/Src/ipcc.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/memorymap.cyclo ./Core/Src/memorymap.d ./Core/Src/memorymap.o ./Core/Src/memorymap.su ./Core/Src/nmea_parse.cyclo ./Core/Src/nmea_parse.d ./Core/Src/nmea_parse.o ./Core/Src/nmea_parse.su ./Core/Src/pulse_oximeter.cyclo ./Core/Src/pulse_oximeter.d ./Core/Src/pulse_oximeter.o ./Core/Src/pulse_oximeter.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/ssd1306.cyclo ./Core/Src/ssd1306.d ./Core/Src/ssd1306.o ./Core/Src/ssd1306.su ./Core/Src/ssd1306_fonts.cyclo ./Core/Src/ssd1306_fonts.d ./Core/Src/ssd1306_fonts.o ./Core/Src/ssd1306_fonts.su ./Core/Src/statemachine.cyclo ./Core/Src/statemachine.d ./Core/Src/statemachine.o ./Core/Src/statemachine.su ./Core/Src/stm32wbxx_hal_msp.cyclo ./Core/Src/stm32wbxx_hal_msp.d ./Core/Src/stm32wbxx_hal_msp.o ./Core/Src/stm32wbxx_hal_msp.su ./Core/Src/stm32wbxx_it.cyclo ./Core/Src/stm32wbxx_it.d ./Core/Src/stm32wbxx_it.o ./Core/Src/stm32wbxx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32wbxx.cyclo ./Core/Src/system_stm32wbxx.d ./Core/Src/system_stm32wbxx.o ./Core/Src/system_stm32wbxx.su ./Core/Src/tarvos.cyclo ./Core/Src/tarvos.d ./Core/Src/tarvos.o ./Core/Src/tarvos.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/usb.cyclo ./Core/Src/usb.d ./Core/Src/usb.o ./Core/Src/usb.su

.PHONY: clean-Core-2f-Src

