rm *.o

arm-none-eabi-gcc -c VS4210.c -mthumb -mcpu=cortex-m3 -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -I./ -I../VS4210 -I../sw_i2c
#/opt/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-gcc -c -O0 VS4210.c -I../VS4210Inc
arm-none-eabi-ar rcs ../Lib/libVS4210.a VS4210.o


