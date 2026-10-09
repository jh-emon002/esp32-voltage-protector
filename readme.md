# ESP32 Smart Voltage and Overcurrent Protection System

**FSMB Recruitment 2026 - Phase 1 | Embedded System Engineer**

This project uses an **ESP32** to demonstrate voltage and overcurrent protection using a **Finite State Machine (FSM)**. Two potentiometers simulate voltage and current readings. A 16x2 LCD displays the readings and system status, LEDs indicate whether the load is enabled or tripped, and pushbuttons or a Wi-Fi web dashboard can change the protection settings.

> **Safety note:** This is a low-voltage demonstration. The potentiometers do not measure real power-grid voltage or current, and the LEDs only indicate the intended load state. The prototype does not physically disconnect a mains-powered appliance. Do not connect it to AC mains.

### Hardware Connections Table

| **Component** | **Pin Connections** |
| --- | --- |
| **Voltage Potentiometer** | Center/wiper: GPIO 34; outer pins: 3.3V and GND |
| **Current Potentiometer** | Center/wiper: GPIO 35; outer pins: 3.3V and GND |
| **16x2 I2C LCD** | SDA: GPIO 21; SCL: GPIO 22; I2C address: `0x27` |
| **Green LED (Normal / Load ON)** | GPIO 25 (through a current-limiting resistor) |
| **Red LED (Fault / Load OFF)** | GPIO 26 (through a current-limiting resistor) |
| **MODE Pushbutton** | GPIO 18 and GND |
| **UP Pushbutton** | GPIO 19 and GND |
| **DOWN Pushbutton** | GPIO 23 and GND |
| **ESP32 Board** | Powered by USB for programming and operation |

### Important Hardware Notes

1. Connect both potentiometers between **3.3V and GND**. Do not apply 5V to the ESP32 analog inputs.
2. The pushbuttons connect to **GND** because the firmware uses `INPUT_PULLUP`. A pressed button reads `LOW`.
3. Use a **220-330 ohm resistor** in series with each LED and connect the LED return to GND.
4. The LCD uses the fixed I2C address **`0x27`**. Make sure the I2C signal lines are at **3.3V logic levels**; use a level shifter if the LCD backpack pulls SDA/SCL up to 5V.
5. All parts connected to the ESP32 must share a **common GND**.

### Instructions to Run the Project

1. **Download the repository:**
   - Open [ESP32 Voltage Protector on GitHub](https://github.com/jh-emon002/esp32-voltage-protector) and select **Code > Download ZIP**, or clone it:
     ```bash
     git clone https://github.com/jh-emon002/esp32-voltage-protector.git
     ```
2. **Open the project:**
   - Open `src/ESP32VoltageProtector.ino` in the **Arduino IDE**. If Arduino IDE asks to place the sketch in its own folder, allow it.
3. **Install required software and libraries:**
   - Install **ESP32 by Espressif Systems** using Arduino IDE's **Boards Manager**.
   - Install a compatible **LiquidCrystal_I2C** library using **Library Manager**.
   - `Wire.h`, `WiFi.h`, and `WebServer.h` are supplied by the Arduino / ESP32 environment.
4. **Connect the hardware:**
   - Follow the **Hardware Connections Table** above.
5. **Upload the code:**
   - Select **ESP32 Dev Module** and the correct COM/serial port.
   - Upload the sketch, then open **Serial Monitor** at **115200 baud**.
6. **Connect to the Wi-Fi dashboard:**
   - Connect a phone or laptop to the ESP32's Wi-Fi network:
     - **SSID:** `VoltageProtector`
     - **Password:** `Protect2026`
   - Open **http://192.168.4.1** in a browser. No internet connection is required. If prompted, choose to remain connected to this Wi-Fi network.
7. **Operate the system:**
   - Rotate the potentiometers to simulate different voltage and current values.
   - Read the values and protection status on the LCD or dashboard.
   - Change the protection limits using the pushbuttons or dashboard, and reset a cleared fault using either interface.

### Pushbutton Controls

| **Control** | **Function** |
| --- | --- |
| **MODE - short press** | Switch between monitoring, voltage-limit, and current-limit LCD pages |
| **UP** | Increase the selected limit |
| **DOWN** | Decrease the selected limit |
| **MODE - hold for about 1.2 seconds** | Reset a fault after the system reaches `RESET_READY` |

### FSM Operation

1. **INIT:** The controller reads the simulated inputs at startup.
2. **NORMAL:** Readings are within the set limits; the **green LED is ON**.
3. **TRIPPED:** Voltage or current exceeds its limit; the **green LED turns OFF** and the **red LED turns ON**. The fault remains latched.
4. **RESET_READY:** The inputs stay below the recovery limits for **1 second**; the **red LED blinks**. The user can now reset the protection system.

The default limits are **250 V** and **8 A**. The simulated input ranges are **0-300 V** and **0-15 A**. The dashboard displays live readings, protection status, fault reason, and adjustable limits. Settings return to their defaults after restarting the ESP32 because they are stored in RAM.

### Repository Files

- `src/ESP32VoltageProtector.ino` - ESP32 firmware and built-in web dashboard.
- `design_document.pdf` - technical design report.
- `schematics/` - wiring and FSM documentation.
- `test_evidence/` - hardware photos, screenshots, and test evidence.
