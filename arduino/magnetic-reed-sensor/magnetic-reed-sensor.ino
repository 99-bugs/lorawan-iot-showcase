#include <Sodaq_RN2483.h>

#define debugSerial SerialUSB
#define loraSerial Serial2

const int REED_PIN = 15;   // Pin van Reed Contact

//**********************************************************
// TODO: verander de waarden van DevAddr, APPkey en NWKkey
//**********************************************************
const uint8_t devAddr[4] = { 0x26, 0x01, 0x1C, 0xEC };
const uint8_t appSKey[16] = { 0x5A, 0x0E, 0xAD, 0x48, 0x09, 0xE8, 0xDE, 0x55, 0xEF, 0x20, 0x41, 0xF7, 0x01, 0x14, 0xA1, 0x2C };
const uint8_t nwkSKey[16] = { 0x07, 0xE9, 0x25, 0xC3, 0x3C, 0x76, 0xA8, 0x61, 0x3A, 0xAA, 0xFD, 0x83, 0xF4, 0xBA, 0x57, 0x7D };

//**********************************************************
// TODO: De poort waarop de data wordt verzonden
//       Andere poort per type sensor
//**********************************************************
const int LORAWAN_PORT = 31;

//**********************************************************
// WARNING:   Niet aanpassen. Maakt de buffer voor data.
//**********************************************************
const int SIZE_OF_BUFFER = 32;
uint8_t buffer[SIZE_OF_BUFFER];
uint8_t numberOfDataBytes = 1;

//**********************************************************
// TODO: De setup van Arduino, wordt in het begin van je
//       sketch 1x uitgevoerd.
//       Als je sensor moet initializeren, doe je dit hier
//**********************************************************
void setup()
{
    pinMode(LED_BLUE, OUTPUT);         // Blauwe LED als uitgang
    pinMode(LED_RED, OUTPUT);          // Rode LED als uitgang
    pinMode(LED_GREEN, OUTPUT);        // Groene LED als uitgang

    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    
    SerialUSB.begin(115200);
    while ((!SerialUSB) && (millis() < 5000));
    SerialUSB.println("Starten van switch demo");

    pinMode(REED_PIN, INPUT_PULLUP);        // Digitale pin als ingang

    // Configuratie van LoRaWAN
    loraSerial.begin(LoRaBee.getDefaultBaudRate());
    LoRaBee.init(loraSerial, LORA_RESET);
    setupLoRa();
}

//**********************************************************
// TODO: De loop van Arduino, deze blijft telkens herhalen
//       Hier kies je een type sensor:
//           - perdiodiek uitgelezen (met delay erin)
//           - event gebasseerde sensoren (zonder delay erin)
//**********************************************************
void loop()
{
    waitForEvent();

    // Verzenden met LoRa
    sendWithLoRa();
}

//**********************************************************
// TODO: Wachten op een verandering voor een event gebasseerde
//       sensor.
//**********************************************************
void waitForEvent()
{
  // Lees de huidige stand van de schakelaar
  int previousState = digitalRead(REED_PIN);
  int state = previousState;

  SerialUSB.println("Wachten voor event");

  // Wachten op verandering van de staat van de schakelaar.
  while (state == previousState) {
    previousState = state;          // Nieuwe staat opslaan in oude staat
    state = digitalRead(REED_PIN);    // Nieuwe staat inlezen
    delay(50);    // Even wachten voor ontdendering
  }

  SerialUSB.print("De schakelaar is ");
  SerialUSB.println(state);

  // Opslaan in buffer om te verzenden
  buffer[0] = (state == HIGH ? 0xFF : 0x00);
  numberOfDataBytes = 1;
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
