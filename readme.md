# ESP32 Smart Voltage Protector - Phase 1

This is a low-voltage prototype for the FSMB 2026 Embedded System Engineer recruitment project. Two potentiometers **simulate** grid voltage and load current. LEDs show the *intended* load state.

## Quick start

1. Open `src/ESP32VoltageProtector.ino` in Arduino IDE.
2. Install the **ESP32 by Espressif Systems** board package and a compatible **LiquidCrystal_I2C** library.
3. Select **ESP32 Dev Module**, choose the correct port, then upload.
4. Open Serial Monitor at **115200 baud**.
5. Connect your phone/laptop to `VoltageProtector` (password `Protect2026`).
6. Open `http://192.168.4.1` in a browser. No internet connection is required.

## Hardware connections

| Part | ESP32 GPIO |
|---|---|
| Voltage potentiometer center pin | 34 |
| Current potentiometer center pin | 35 |
| I2C LCD (16x2, address `0x27`) SDA / SCL | 21 / 22 |
| Green LED / Red LED | 25 / 26 |
| MODE / UP / DOWN buttons | 18 / 19 / 23 |


## Operation

- **NORMAL**: Green LED ON, red LED OFF.
- **TRIPPED**: Any reading exceeding its limit latches the fault; green OFF, red ON.
- **RESET READY**: After voltage and current remain below the recovery limits for 1 second, green stays OFF and red blinks.
- **RESET**: Hold MODE for about 1.2 seconds or use the web Reset button when reset-ready.
- Short MODE switches LCD pages. UP and DOWN change limits on settings pages.
- Web dashboard shows voltage, current, fault, load indicator and settings; it also allows threshold changes and reset.

simulated ranges: **0-300 V** and **0-15 A**. Settings are in RAM and return to default after restarting the ESP32.

## Repository files

- `src/` - complete Arduino sketch, including Wi-Fi page
- `design_document.pdf` - simple one-page report with reserved FSM-diagram space
- `schematics/` - wiring notes and instructions for adding an FSM diagram
- `test_evidence/` - place your actual circuit photos and dashboard screenshots here

