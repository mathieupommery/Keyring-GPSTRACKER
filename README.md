# Keyring-GPSTRACKER

# next update

switch to stm32wb55: adding bluetooth !
adding wsan pads barometer for precise altitude values.
create external I2C sensor with MAX30102 and MAX32664 heart rate ppg sensor and hub.

# NEWS
-With v1.0 a 128Mb flash memory is onboard in order to store data in csv format and you can send them to computer by com port! On top of that there is two jst sh port to add i2c or uart external sensor powered by the keyring gps(3.3V and 50mA max power consuption) ! 

render of the pcb final version (v3):

![v3 top](https://github.com/user-attachments/assets/7c62bd4c-bef6-4dc5-8d68-f8357025eb33)
![v3 bottom](https://github.com/user-attachments/assets/0ff3f03d-6d4a-4fff-99a5-071ec242e075)





# Specs(v3)
* STM32L432 MCU 
* oled 0.49 inch display
* 2 button to change data on screen
* firmware update throught USB C by DFU mode
* 1H charging time for 2h autonomy min (continuous fix and store data) much more if you turn off gps by the mosfet onboard when it isn't used. 
* vbat indicator
* up to 128 Mo flash memory (100H+ data storage)
* 2 external JST-SH sensor port (1 UART, 1 I2C ) for heart rate purpose for example.
* Can measure and store: mcu temp; vbat ; speed; lat; long ; alt ; sat number; hdop; time (utc and built in chrono) 

# ALL PCB VERSION:
# v1(obsolete)
![v1 top](https://github.com/user-attachments/assets/84e36593-c286-4661-8d89-52ee60501dd8)
![v1 bottom](https://github.com/user-attachments/assets/6fc9801c-9bce-44b1-aaf7-508593e083b5)
![v1 sidev](https://github.com/user-attachments/assets/250443ae-6e7f-4088-a745-f3e589df8542)

# v2(compas inside but useless due to emi)
![v2 top](https://github.com/user-attachments/assets/5a7bca42-62a9-42c2-8e30-4d8c6f509ca4)
![v2 bottom](https://github.com/user-attachments/assets/066c112f-2884-4bc9-9c6f-ac67cdb4ba49)
![v2 side](https://github.com/user-attachments/assets/1ff55117-55c8-4810-943c-c2a338e8ec45)

# v3(up to 128Mo flash memory ! and 2 ext port)

![v3 top](https://github.com/user-attachments/assets/4a4b44ef-3b8c-4360-8704-396627efd014)
![v3 bottom](https://github.com/user-attachments/assets/cd00da08-a375-4a12-9ec0-8f4e382e86c1)
![v3 side](https://github.com/user-attachments/assets/8e459958-e515-4949-8b18-d1f3be7d769c)



# Some pictures:

![IMG_6612](https://github.com/mathieupommery/Keyring-GPSTRACKER/assets/161692882/6084d13b-143e-4e49-94e5-a6b4e5ec9856)

here you can see the starting screen.

*And a video that show it working (v0.2)

https://github.com/mathieupommery/Keyring-GPSTRACKER/assets/161692882/a2b60fe9-0aff-438f-a0ec-e4cd5b23454a

