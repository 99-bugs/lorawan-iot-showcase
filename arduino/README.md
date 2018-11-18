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
RFID | 25 | 12 bytes: Id in ASCII |

## Libraries Required

### RFID

* [https://github.com/SodaqMoja/Sodaq_RN2483.git](https://github.com/SodaqMoja/Sodaq_RN2483.git)

### Ultrasonic Ranger

```shell
cd ~/Arduino/libraries
git clone https://github.com/Seeed-Studio/Grove_Ultrasonic_Ranger.git
git checkout v1.0.1
```
