# Keyring-GPSTRACKER (v12.0)

<p align="center">
  <video src="https://github.com/user-attachments/assets/1a3d3f26-0a68-42e8-81eb-b6b98553143f" width="600" controls autoplay loop muted></video>
</p>

An ultra-compact, highly integrated modular GPS logger built around the STM32G0B1KE and a custom u-blox MIA-M10Q implementation. This project is designed as a fully functional, wearable GNSS tracker featuring worldwide offline geographical awareness, high-frequency SD card logging, Bluetooth connectivity, extreme power management (2µA shutdown draw), and modular waterproof enclosures.

## Key Features

* **Configurable High-Speed Logging:** Records tracks directly to the internal MicroSD card in `.GPX` or `.CSV` format. The logging frequency is adjustable, supporting real-time capture from **1Hz up to 10Hz**.
* **Worldwide Offline Awareness & Navigation:** Anywhere in the world, the device can instantly identify the closest city (name, direction, population) or the nearest mountain peak. It provides precise directional pointers by fusing data from the GPS, the MMC5603 compass, and the ICM-42688-P IMU.
* **Topological Barometer Calibration:** Future firmware implementation of an offline map of topological elevation points stored on the SD card, allowing ultra-precise, automatic recalibration of the barometer altitude.
* **BLE Smartphone Connectivity:** An external **DA14531 Bluetooth Low Energy (BLE)** module will handle seamless, low-power synchronization and configuration with a smartphone app.
* **Plug & Play USB:** Acts as a USB Mass Storage Class (MSC) over USB-C. Simply plug the tracker into a computer to access the MicroSD card and retrieve your tracks without removing the card.
* **Extreme Low Power:** Fully active power draw is around ~30mA. In shutdown mode, with only 1 LDO running for the STM32, the system draws just **2µA**.

---

## Modular Hardware Ecosystem (4 Custom PCBs)

The project relies on an ecosystem of four custom PCBs (two 6-layer boards and two 2-layer boards) connected via ultra-compact 5-pin board-to-board connectors.

### 1. Mainboard (6-Layer)
The core of the tracker, engineered to maximize available I/O ports while maintaining a tiny footprint.
* **MCU:** STM32G0B1KE (chosen for native USB FS 2.0, ultra-low power shutdown modes, and only 1 VDD pin).
* **Internal Sensors:** MMC5603 (compass), ICM-42688-P (IMU), and DPS368 (barometer).
* **Storage & Connectivity:** MicroSD card slot operating in SPI mode + external DA14531 BLE module support.
* **Power & Charging:** LTC4065 LiPo battery charger with USB-C connector.

<p align="center">
  <img width="48%" alt="v12side" src="https://github.com/user-attachments/assets/96f011da-12fb-4d81-9a8f-0a27fd8f81d3" />
  <img width="48%" alt="v12top" src="https://github.com/user-attachments/assets/7a0c73cc-767e-4207-bbfc-9a4213295369" />
</p>

#### BLE Expansion Module (DA14531)
To keep the mainboard ultra-compact while allowing wireless smartphone communication, wireless connectivity is offloaded to a dedicated mini extension PCB. Connected directly to the back of the mainboard via a board-to-board connector, this add-on integrates the ultra-low-power **Renesas DA14531 CodeLess BLE module**.

<p align="center">
  <img width="48%" alt="Mainboard Back - Without BLE Module" src="https://github.com/user-attachments/assets/fd9b3578-7140-435d-9f69-262bb5a640db" />
  <img width="48%" alt="backv12withda14531" src="https://github.com/user-attachments/assets/2f8fd850-f3b8-40d5-a29a-017e2e539ae4" />

</p>


### 2. Custom GNSS Receiver Board (6-Layer)
A custom BGA adaptation of the u-blox **MIA-M10Q** receiver with a dedicated SAW filter and a top-mounted ceramic patch antenna. Designed as a footprint clone of the PA1010D, but utilizing a far superior engine capable of 10Hz updates across all constellations simultaneously.

<img width="360" height="356" alt="gpstop" src="https://github.com/user-attachments/assets/482acbf6-33d1-4602-9580-7ad055b9bab4" />
<img width="386" height="359" alt="gpsbottom" src="https://github.com/user-attachments/assets/e35c34c4-8518-4288-a0fb-1d677d408776" />

### 3. Display & Waterproof Extension Panel (Breakaway 2-Layer PCB)

To optimize manufacturing, all auxiliary modules are designed as a single **snap-away (breakaway) panelized 2-layer PCB** so they can be produced together in one manufacturing run. Once snapped apart, the panel yields three distinct functional boards:

* **0.42" OLED Carrier Board:** Adapts a 0.42-inch SSD1306 OLED screen to the custom 5/6-pin board-to-board (B2B) connector format. It also breaks out the I2C bus to an external JST SM05B connector and includes an external GPIO breakout for a hardware button.
* **0.49" OLED Carrier Board:** Identical architecture to the 0.42" version (B2B adaptation, JST SM05B I2C breakout, button GPIO), but specifically dimensioned to mount a slightly larger 0.49-inch SSD1306 OLED screen.
* **Waterproof Dive Extension Board:** Designed specifically to mount inside the waterproof enclosure for deep-water excursions. It adds a **TE Connectivity 30-bar external pressure sensor** (effectively turning the tracker into a compact dive computer) and a magnetic **Hall-effect switch** for contactless power/stop-mode toggling through the solid sealed case.

<p align="center">
  <img width="48%" alt="auxpcbside2" src="https://github.com/user-attachments/assets/21618c5a-887b-433a-8204-c9117be2f45f" />
  <img width="48%" alt="auxpcbside1" src="https://github.com/user-attachments/assets/051a96b4-ca0d-451b-a6a9-978a5d9f2137" />
</p>




## Smart Power Architecture

Power is cleanly isolated and distributed via 3 independent ultra-low IQ LDOs, controlled by the MCU:
* **LDO 1 (Always-on / MCU baseline):** Powers the STM32 directly. Responsible for the 2µA deep shutdown consumption.
* **LDO 2 (Switched):** Dedicated exclusively to the GNSS module.
* **LDO 3 (Switched):** Powers the SSD1306 display, the MicroSD card, external sensors (including the BLE module), and internal sensors.

---

## Enclosure System: Standard vs. Waterproof (Dive Mode)

The tracker features a modular, tool-free swap enclosure system depending on your activity:

### 1. Standard Compact Case
The daily-use enclosure. It is ultra-compact, non-waterproof, and provides direct access to the OLED display, physical buttons, MicroSD card slot, and USB-C port for charging and mass-storage data transfer.

<img width="1530" height="2040" alt="balise" src="https://github.com/user-attachments/assets/76b08611-f62c-4e0c-b786-7c5770ad0383" />

### 2. Waterproof Dive Case
Similar to a GoPro housing, this heavy-duty case is designed for underwater excursions down to several hundred meters. 
* **How it works:** You remove the mainboard from the standard case, connect it to the **Waterproof Extension Board** (with the 30-bar sensor and Hall switch already mounted inside the dive case), turn it on, and seal the enclosure.
* **Note:** While in this temporary dive case, USB charging and physical MicroSD card access are blocked to ensure 100% water tightness. Power state is managed entirely through the external magnetic Hall sensor.

<img width="459" height="386" alt="watercase1" src="https://github.com/user-attachments/assets/f1433ccb-cddb-4526-bd76-79de9665180a" />

---

## Open Source

This project is fully open source! Feel free to explore the schematics, layouts, and firmware. If you find this project interesting or build your own, **please consider leaving a ⭐ on the repository!**
