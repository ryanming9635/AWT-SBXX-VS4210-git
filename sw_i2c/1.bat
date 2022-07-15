rm *.o

arm-none-eabi-gcc -c sw_i2c0.c -mthumb -mcpu=cortex-m3 -O0 -Wall -fdata-sections -ffunction-sections -DUSE_HAL_DRIVER -DSTM32F103xB -I./ -I../VS4210  -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Inc  -I../VS9988
arm-none-eabi-ar rcs ../Lib/libsw_i2c0.a sw_i2c0.o

arm-none-eabi-gcc -c sw_i2c1.c -mthumb -mcpu=cortex-m3 -O0 -Wall -fdata-sections -ffunction-sections -DUSE_HAL_DRIVER -DSTM32F103xB -I./ -I../VS4210  -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Inc -I../VS9988
arm-none-eabi-ar rcs ../Lib/libsw_i2c1.a sw_i2c1.o

arm-none-eabi-gcc -c sw_i2c2.c -mthumb -mcpu=cortex-m3 -O0 -Wall -fdata-sections -ffunction-sections -DUSE_HAL_DRIVER -DSTM32F103xB -I./ -I../VS4210  -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Inc -I../VS9988
arm-none-eabi-ar rcs ../Lib/libsw_i2c2.a sw_i2c2.o

arm-none-eabi-gcc -c sw_i2c3.c -mthumb -mcpu=cortex-m3 -O0 -Wall -fdata-sections -ffunction-sections -DUSE_HAL_DRIVER -DSTM32F103xB -I./ -I../VS4210  -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Inc -I../VS9988
arm-none-eabi-ar rcs ../Lib/libsw_i2c3.a sw_i2c3.o

arm-none-eabi-gcc -c sw_i2c4.c -mthumb -mcpu=cortex-m3 -O0 -Wall -fdata-sections -ffunction-sections -DUSE_HAL_DRIVER -DSTM32F103xB -I./ -I../VS4210  -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Inc -I../VS9988
arm-none-eabi-ar rcs ../Lib/libsw_i2c4.a sw_i2c4.o


pause 
