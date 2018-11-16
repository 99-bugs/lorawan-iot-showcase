#include <Sodaq_RN2483.h>

#define debugSerial SerialUSB
#define loraSerial Serial2

//**********************************************************
// TODO: verander de waarden van DevAddr, APPkey en NWKkey
//**********************************************************
const uint8_t devAddr[4] = { 0x26, 0x01, 0x1C, 0xBB };
const uint8_t appSKey[16] = { 0xB7, 0xD5, 0xFA, 0xB2, 0x4B, 0xAA, 0x46, 0xBB, 0xBE, 0xA3, 0x3F, 0x8C, 0x01, 0x49, 0x80, 0x05 };
const uint8_t nwkSKey[16] = { 0xE7, 0x7F, 0x6E, 0x8A, 0x68, 0x4A, 0x86, 0xCB, 0xFF, 0x8E, 0x7E, 0x07, 0x47, 0x6A, 0xA4, 0xBC };

//**********************************************************
// TODO: De poort waarop de data wordt verzonden
//       Andere poort per type sensor
//**********************************************************
const int LORAWAN_PORT = 25;

//**********************************************************
// WARNING:   Niet aanpassen. Maakt de buffer voor data.
//**********************************************************
const int SIZE_OF_BUFFER = 32;
uint8_t buffer[SIZE_OF_BUFFER];
uint8_t numberOfDataBytes = 12;

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
    debugSerial.println("Starten van RFID demo");

    // Configuratie van LoRaWAN
    loraSerial.begin(LoRaBee.getDefaultBaudRate());
    LoRaBee.init(loraSerial, LORA_RESET);
    setupLoRa();

    // Serial is verbonden met de RFID lezer
    Serial.begin(9600);
}

//**********************************************************
// TODO: De loop van Arduino, deze blijft telkens herhalen
//       Hier kies je een type sensor:
//           - perdiodiek uitgelezen (met delay erin)
//           - event gebasseerde sensoren (zonder delay erin)
//**********************************************************
void loop()
{
    // Lees het id van een RFID kaart
    read_rfid_card();   // Blocking
    
    SerialUSB.print("Kaart gedetecteerd met ID = ");
    SerialUSB.println((char*)buffer);

    // Verzenden met LoRa
    sendWithLoRa();
}

//**********************************************************
// Read value from RFID
//**********************************************************

int  read_rfid_card() {
  static int counter = 0;       // Een teller voor buffer
  int sizeOfId = 0;

  while (sizeOfId != 12) {
    while(Serial.available()) {
      char newChar = Serial.read();   // Lees karakter van Serial
      // ASCII 02: STX (Start of Text)
      // ASCII 03: ETX (End of Text)
      if (newChar != 2 && newChar != 3 && counter < SIZE_OF_BUFFER) {
        buffer[counter++] = newChar;
        buffer[counter] = '\0';
        sizeOfId = counter;
      } else {
        counter = 0;
      }
    }
  }
 
  return sizeOfId;
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