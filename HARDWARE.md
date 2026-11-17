# Hardware Used

| # | Component | Role in the project | Photo |
|---|---|---|---|
| 1 | **NodeMCU ESP8266** (ESP-12E) | Main controller with built-in Wi-Fi; runs the firmware and web dashboard | [full setup](images/01-full-prototype-setup.png) |
| 2 | **DHT11** temperature & humidity sensor | Measures air temperature and relative humidity (data pin D4) | [full setup](images/01-full-prototype-setup.png) |
| 3 | **Soil moisture probe** | Senses soil dryness | [photo](images/03-soil-moisture-probe.png) |
| 4 | **Soil sensor comparator module** (LM393-type) | Conditions the probe signal, analog output to A0 | [photo](images/04-soil-sensor-comparator-module.png) |
| 5 | **Dual motor driver module** | Switches the pump from NodeMCU pins D5 / D6 | [photo](images/05-dual-motor-driver-module.png) |
| 6 | **Mini submersible water pump** + silicone tube | Delivers water | [photo](images/06-submersible-water-pump.png) |
| 7 | **0.96" OLED display** (SH1106, I2C) | Shows temperature, humidity, soil value, mode, pump state | [photo](images/02-oled-live-readings.png) |
| 8 | Breadboard, jumper wires, USB cable | Prototyping and connections | [full setup](images/01-full-prototype-setup.png) |
| 9 | 5 V regulated power supply | Powers the circuit | [full setup](images/01-full-prototype-setup.png) |

Water test of the probe: [photo](images/07-soil-probe-water-test.png)
