rm *.o

arm-none-eabi-gcc -c tx_csc.c  hdmitx_drv.c hdmitx_input.c hdmitx_sys.c  -mthumb -mcpu=cortex-m3 -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 
arm-none-eabi-ar rcs ../Lib/libite66121dvi.a tx_csc.o  hdmitx_drv.o hdmitx_input.o hdmitx_sys.o


