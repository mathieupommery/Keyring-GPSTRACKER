# Keyring-GPSTRACKER

This is a keyring gps balise with onboard compas, gps, stm32l4 mcu, aoled 0.49inch display and a lipo battery charger,
you can flash a new firmware throught dfu mode by the usb c(push the little button on the back of the card whe you plug in the usb to turn the mcu in dfu mode) or by swd solder pad and a stlink if you want to debug also.
Usb c can also be used to charge the li ion battery inside.

a mosfet can enable or disable the gps power in order to save battery when it isn't used, the power consuption can drop below 50uA with just the mcu in low power state running !

##it currently has 3 main use: 

-In order to get a small device linked to your key in order to have position, speed,UTC hour and north direction in case if your are lost when you're hiking.


-other use:i implemented in the code a memory storage of the max speed(in kmh) also there is a screen that show you the current speed and max speedsince it started too you can use that as a precise gps speedmeter for rc car, rc plane or real car !


-third use: last screen: choose hour/choose minutes let you choose a precise houre and minutes, and when it connect to the gps system,and if your are connected to the gps system, it start a countdown, when it is at zero, it activate a gpio pin on the back of the pcb that you can connect to pretty much everything.

Some pictures:

![IMG_6612](https://github.com/mathieupommery/Keyring-GPSTRACKER/assets/161692882/6084d13b-143e-4e49-94e5-a6b4e5ec9856)

here you can see the starting screen.




*And a video that show it working

https://github.com/mathieupommery/Keyring-GPSTRACKER/assets/161692882/a2b60fe9-0aff-438f-a0ec-e4cd5b23454a

this video didn't show how to choose your hour and minutes for the countdown, in order to do that, you have to go in the choose your houre menu by pressing A button for selecting the menu and then you press a to increase the hour, then you press b to choose the minutes, and the you press a to increase the minutes, finally you press b to valid.

