# LoRaWAN Sensor documentation

## Port numbers and data formats

Sensor | Port | Data format
---------|----------|---
Temperature demo (SODAQ internal) | 1 | 2 bytes
TPH | 10 | 6 bytes: [ 2 bytes Temperature, 3 bytes pressure, 1 byte humidity]
Rotation sensor | 11 | 1 byte Rotation (0-255)
Airquality sensor | 12 | 2 bytes, 10 bit (0-1024) 
Loudness sensor | 15 | 2 bytes, 10 bit (0-1024)
Pir sensor | 16 | 1 byte, motions per minute
Push button | 17 | 1 byte, 0x01 == event
RFID | 25 | 12 bytes: Id in ASCII |
Ultrasonic Distance | 26 | 2 bytes: 0 - 400 cm |
SPDT Switch | 27 | 1 byte: 0x00 or 0xFF |
Tilt sensor | 28 | 1 byte: 0x00 or 0xFF |
Temperature and Humidity Pro sensor | 29 | 4 bytes: 2 bytes Temperature, 2 bytes humidity |
Gas sensor | 30 | 2 bytes gas density (value between 0 and 1000; divide by 1000 to get 0.0 to 1.0) |
Magnetic Reed sensor | 31 | 1 byte: 0x00 or 0xFF |

## Libraries Required

### RFID

* [https://github.com/SodaqMoja/Sodaq_RN2483.git](https://github.com/SodaqMoja/Sodaq_RN2483.git)

### Ultrasonic Ranger

```shell
cd ~/Arduino/libraries
git clone https://github.com/Seeed-Studio/Grove_Ultrasonic_Ranger.git
git checkout v1.0.1
```

### Temperature and Humidity Pro

```shell
cd ~/Arduino/libraries
git clone https://github.com/BioBoost/Grove_Temperature_And_Humidity_Sensor.git
```