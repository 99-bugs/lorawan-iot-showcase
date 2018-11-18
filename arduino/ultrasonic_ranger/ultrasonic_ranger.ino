#include "Ultrasonic.h"
#include <Sodaq_RN2483.h>

#define loraSerial Serial2
#define debugSerial SerialUSB
#define SONIC_PIN 15
#define MEASURE_DELAY 500
#define LORA_SEND_DELAY 10000

Ultrasonic ultrasonic(SONIC_PIN);

//**********************************************************
// TODO: verander de waarden van DevAddr, APPkey en NWKkey
//**********************************************************
const uint8_t devAddr[4] = { 0x26, 0x01, 0x13, 0x46 };
const uint8_t appSKey[16] = { 0x0C, 0xEE, 0x12, 0xAC, 0xDD, 0x51, 0x3E, 0xE1, 0x11, 0xED, 0x1F, 0xCE, 0x68, 0xEF, 0x30, 0x5C };
const uint8_t nwkSKey[16] = { 0x2C, 0xAF, 0x33, 0x4E, 0xD1, 0xD8, 0x31, 0x99, 0x46, 0xDD, 0xEA, 0xEF, 0x76, 0xED, 0x0A, 0x60 };

//**********************************************************
// TODO: De poort waarop de data wordt verzonden
//       Andere poort per type sensor
//**********************************************************
const int LORAWAN_PORT = 26;

//**********************************************************
// WARNING:   Niet aanpassen. Maakt de buffer voor data.
//**********************************************************
const int SIZE_OF_BUFFER = 32;
uint8_t buffer[SIZE_OF_BUFFER];
uint8_t numberOfDataBytes = 2;

//**********************************************************
// TODO: De setup van Arduino, wordt in het begin van je
//       sketch 1x uitgevoerd.
//       Als je sensor moet initializeren, doe je dit hier
//**********************************************************
void setup() {
    pinMode(LED_BLUE, OUTPUT);         // Blauwe LED als uitgang
    pinMode(LED_RED, OUTPUT);          // Rode LED als uitgang
    pinMode(LED_GREEN, OUTPUT);        // Groene LED als uitgang

    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);

    debugSerial.begin(115200);
    while ((!debugSerial) && (millis() < 5000));
    debugSerial.println("Starting Ultrasonic Ranger Demo");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Configuratie van LoRaWAN
    loraSerial.begin(LoRaBee.getDefaultBaudRate());
    LoRaBee.init(loraSerial, LORA_RESET);
    setupLoRa();
}

void loop() {
  static int loopCounter = 0;

  debugSerial.println("The distance to obstacles in front is: ");
  long range = ultrasonic.MeasureInCentimeters();    // two measurements should keep an interval
  debugSerial.print(range);  //0~400cm
  debugSerial.println(" cm");

  int timesBlink = (MEASURE_DELAY) / (10*(range+1));    // Small range, fast blink, x10 to slow down a bit, +1 to fix divide by zero
  int deltaDelay = MEASURE_DELAY / (2*timesBlink);

  if (timesBlink < 1) {
    delay(MEASURE_DELAY);
  }
  for (int i = 0; i < timesBlink; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(deltaDelay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(deltaDelay);
  }

  loopCounter++;
  if (loopCounter >= (LORA_SEND_DELAY / MEASURE_DELAY)) {
    buffer[0] = range & 0xFF;
    buffer[1] = (range >> 8) & 0xFF;
    // Verzenden met LoRa
    sendWithLoRa();
    loopCounter = 0;
  }
}

//**********************************************************
// WARNING:   Vanaf hier dien je niets meer aan te passen.
//            Dit zijn de functies die de LoRa data verzenden.
//**********************************************************
void setupLoRa()
{
    if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true)) {
        debugSerial.println("Network connection successful.");
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
    }
    else {
        debugSerial.println("Network connection failed!");
    }
    LoRaBee.setSpreadingFactor(7);
}

void sendWithLoRa() {
    switch (LoRaBee.send(LORAWAN_PORT, buffer, numberOfDataBytes))
    {
        case NoError:
          debugSerial.println("Successful transmission.");
          break;
        case NoResponse:
          debugSerial.println("There was no response from the device.");
          break;
        case Timeout:
          debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
          delay(20000);
          break;
        case PayloadSizeError:
          debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
          break;
        case InternalError:
          debugSerial.println("Oh No! This shouldn not happen. Something is really wrong! The program will reset the RN module.");
          setupLoRa();
          break;
        case Busy:
          debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
          delay(10000);
          break;
        case NetworkFatalError:
          debugSerial.println("There is a non-recoverable error with the network connection. The program will reset the RN module.");
          setupLoRa();
          break;
        case NotConnected:
          debugSerial.println("The device is not connected to the network. The program will reset the RN module.");
          setupLoRa();
          break;
        case NoAcknowledgment:
          debugSerial.println("There was no acknowledgment sent back!");
          break;
        default:
          break;
    }
}
