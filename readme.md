# ESP32 Smart Voltage Protector - Phase 1

This is a low-voltage educational prototype for the FSMB 2026 Embedded System Engineer recruitment project. Two potentiometers **simulate** grid voltage and load current. LEDs show the *intended* load state; they do not physically disconnect an appliance. **Do not connect this build to AC mains.**

## Quick start

1. Open `src/ESP32VoltageProtector/ESP32VoltageProtector.ino` in Arduino IDE.
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

Potentiometers: outer pins to ESP32 3.3 V and GND; never connect these ADC inputs to 5 V. Each LED needs a current-limiting resistor. Buttons connect to GND and use internal pull-ups. Check that your I2C LCD's SDA/SCL pull-up voltage is safe for the ESP32; a 5 V LCD backpack may need an I2C level shifter. See `schematics/wiring.md`.

## Operation

- **NORMAL**: Green LED ON, red LED OFF.
- **TRIPPED**: Any reading exceeding its limit latches the fault; green OFF, red ON.
- **RESET READY**: After voltage and current remain below the recovery limits for 1 second, green stays OFF and red blinks.
- **RESET**: Hold MODE for about 1.2 seconds or use the web Reset button when reset-ready.
- Short MODE switches LCD pages. UP and DOWN change limits on settings pages.
- Web dashboard shows voltage, current, fault, load indicator and settings; it also allows threshold changes and reset.

Default simulated ranges: **0-300 V** and **0-15 A**. Default limits: **250 V** and **8 A**. Settings are in RAM and return to default after restarting the ESP32.

## Repository files

- `src/` - complete Arduino sketch, including Wi-Fi page
- `design_document.pdf` - simple one-page report with reserved FSM-diagram space
- `schematics/` - wiring notes and instructions for adding an FSM diagram
- `test_evidence/` - place your actual circuit photos and dashboard screenshots here
- `project_files/` - editable Word report template (for adding the FSM image)

## Before submission

1. Edit `project_files/design_document_editable.docx` to insert **your own FSM diagram** into the blank box. Export it as the root `design_document.pdf` (replacing the draft).
2. Add actual evidence photos/screenshots to `test_evidence/` and optionally the FSM image to `schematics/`.
3. Record a demonstration video **5 minutes or less and 100 MB or less**, per the assessment. Provide the video through the Google Form.
4. Push this complete project folder to a **public GitHub repository**; share its full `https://...` URL in the submission form.
5. Read the report and personalize the language before submitting; do not claim hardware protections not implemented.

## Current limitations

This is a low-voltage **emulator**, not a mains voltage protector. Potentiometers do not measure physical grid voltage or current. Green LED means "simulated load ON" but is not a relay or an isolated load disconnect. There is no verified mains trip-time measurement, surge suppression, or electrical isolation.
