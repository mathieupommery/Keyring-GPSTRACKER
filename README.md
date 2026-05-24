
# Keyring-GPSTRACKER

An ultra-compact, highly integrated GPS logger built around the STM32L432 and a custom u-blox MIA-M10Q implementation. Measuring just 17 x 22.65 mm on a 6-layer PCB, this project is designed to be a fully functional, wearable GNSS tracker with smart power management and USB mass storage capabilities.

## Hardware Specifications

* **MCU:** STM32L432KCU6 (Cortex-M4, ultra-low power).
* **PCB:** 6-layer high-density board, 17.0 mm x 22.65 mm.
* **GNSS Module:** Custom footprint adaptation of the u-blox MIA-M10Q (10x10 mm LGA) designed to match the PA1010D form factor.
* **Storage:** MicroSD card slot for track logging.
* **Expansion:** Molex board-to-board connector for external sensors.
* **Charging:** LTC4065 Li-Po battery charger.
* **User Interface:** * Support for external OLED display.
  * 3x Status LEDs.
  * 2x Multi-purpose buttons (Button 1: Power/App logic, Button 2: BOOT0/App logic).

## Smart Power Architecture

The system is designed for maximum battery life with a permanent MCU connection to the battery (utilizing deep sleep modes) and modular power domains. 
Power is distributed via 3 independent LDOs, two of which are dynamically controlled by the STM32:
* **LDO 1 (Switched):** Powers the OLED display, the MicroSD card, and the external sensor connector.
* **LDO 2 (Switched):** Dedicated exclusively to the GNSS module.
* **LDO 3 (Always-on):** System baseline.

## Firmware Features

* **Standardized Logging:** Records tracks natively in `.GPX` or `.CSV` formats.
* **Plug & Play USB:** Acts as a USB Mass Storage Class (MSC) device. Simply plug the tracker into a computer via USB to access the MicroSD card and retrieve your tracks without removing the card.

## Gallery (v9)

*(Top View)*
<img width="1640" height="1144" alt="front_v9" src="https://github.com/user-attachments/assets/c4cab1ae-69a4-4ac2-82da-bbb88f001cf2" />


*(Bottom View)*
<img width="1410" height="1124" alt="back_v9" src="https://github.com/user-attachments/assets/a3c8f73e-0dad-49ce-8543-2b1caab1f89e" />

*(Side View)*

<img width="1802" height="786" alt="side_v9" src="https://github.com/user-attachments/assets/908352aa-5bec-4010-8b1a-e481587f9420" />
