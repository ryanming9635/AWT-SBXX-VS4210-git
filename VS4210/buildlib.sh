rm *.o

arm-none-eabi-gcc -c VX_16port_Display.c -mthumb -mcpu=cortex-m3 -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -I./ -I../sw_i2c
#/opt/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-gcc -c -O0 VS4210.c -I../VS4210Inc
arm-none-eabi-ar rcs ../Lib/libVX_16port_Display.a VX_16port_Display.o
