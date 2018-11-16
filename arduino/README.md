# LoRaWAN Sensor documentation

## Port numbers and data formats

Sensor | Port | Data format
---------|----------|---
Temperature demo (SODAQ internal) | 1 | 2 bytes
TPH | 10 | 6 bytes: [ 2 bytes Temperature, 3 bytes pressure, 1 byte humidity]
RFID | 25 | 12 bytes: Id in ASCII |

## Libraries Required

### RFID

* [https://github.com/SodaqMoja/Sodaq_RN2483.git](https://github.com/SodaqMoja/Sodaq_RN2483.git)