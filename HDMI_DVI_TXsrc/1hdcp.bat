rm *.o

arm-none-eabi-gcc -c sha1.c tx_csc.c hdmitx_drv.c hdmitx_input.c hdmitx_sys.c hdmitx_hdcp.c -mthumb -mcpu=cortex-m3 -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 
arm-none-eabi-ar rcs ../Lib/libite66121hdcp.a sha1.o tx_csc.o  hdmitx_drv.o hdmitx_input.o hdmitx_sys.o hdmitx_hdcp.o



