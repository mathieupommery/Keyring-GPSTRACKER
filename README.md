# Keyring-GPSTRACKER

*This is a keyring gps balise with onboard compas, gps, stm32l4 mcu, aoled 0.49inch display and a lipo battery charger,
you can flash a new firmware throught dfu mode by the usb c(push the little button on the back of the card whe you plug in the usb to turn the mcu in dfu mode) or by swd solder pad and a stlink if you want to debug also.
Usb c can also be used to charge the li ion battery inside.

it currently has 3 main use: 

-In order to get a small device linked to your key in order to have position, speed,UTC hour and north direction in case if your are lost when you're hiking.

-other use:i implemented in the code a memory storage of the max speed(in kmh) also there is a screen that show you the current speed and max speedsince it started too you can use that as a precise gps speedmeter for rc car, rc plane or real car !

-third use: last screen: choose hour/choose minutes let you choose a precise houre and minutes, and when it connect to the gps system,and if your are connected to the gps system, it start a countdown, when it is at zero, it activate a gpio pin on the back of the pcb that you can connect to pretty much everything.

Some pictures:

