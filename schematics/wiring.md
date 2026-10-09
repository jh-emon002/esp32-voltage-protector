# Hardware connections

| Part | ESP32 GPIO | Wiring |
| --- | --- | --- |
| Voltage pot wiper | 34 | Outer ends: 3.3V, GND |
| Current pot wiper | 35 | Outer ends: 3.3V, GND |
| LCD 16x2 I2C SDA | 21 | LCD backpack, address 0x27 |
| LCD 16x2 I2C SCL | 22 | LCD backpack, address 0x27 |
| Green LED | 25 | Through ~220-330 ohm resistor to LED/GND |
| Red LED | 26 | Through ~220-330 ohm resistor to LED/GND |
| MODE | 18 | Button to GND |
| UP | 19 | Button to GND |
| DOWN | 23 | Button to GND |

**Safety:** Use only low-voltage ESP32-compatible wiring. A common 5V I2C backpack may put 5V on SDA/SCL; use 3.3V-safe pull-ups or a level shifter as needed. Do not connect this circuit to mains power.

The LEDs are indicators only; they are not a physical electrical disconnect switch. You can add a hand-drawn/photo schematic here later as `circuit.jpg` or `circuit.png`.
