#include <Sodaq_RN2483.h>

#include <Wire.h>

#include "Arduino.h"
#include "SI114X.h"

#define debugSerial SerialUSB
#define loraSerial Serial2

SI114X SI1145 = SI114X();

//**********************************************************
// TODO: verander de waarden van DevAddr, APPkey en NWKkey
//**********************************************************
const uint8_t devAddr[4] = { 0x26, 0x01, 0x10, 0x0E };
const uint8_t appSKey[16] = { 0x50, 0xE1, 0x9A, 0x47, 0xB6, 0xB7, 0x7B, 0xD1, 0x04, 0xE0, 0x1B, 0xB2, 0x80, 0x3C, 0xB8, 0xB5 };
const uint8_t nwkSKey[16] = { 0xC1, 0xD6, 0x2A, 0xAA, 0xAF, 0x51, 0xB0, 0x45, 0x31, 0x9B, 0xC7, 0xA2, 0x17, 0x8D, 0x47, 0x70 };

//**********************************************************
// TODO: De poort waarop de data wordt verzonden
//       Andere poort per type sensor
//**********************************************************
const int LORAWAN_PORT = 21;

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
    debugSerial.println("Starting LoRaWAN");

    // Configuratie button op Sodaq
    pinMode(BUTTON, INPUT);        // Digitale pin als ingang

    // Configuratie van LoRaWAN
    loraSerial.begin(LoRaBee.getDefaultBaudRate());
    LoRaBee.init(loraSerial, LORA_RESET);
    setupLoRa();

    debugSerial.println("Beginning Si1145!");

    while (!SI1145.Begin()) {
      debugSerial.println("Si1145 is not ready!");
      delay(1000);
    }
    debugSerial.println("Si1145 is ready!");
}

//**********************************************************
// TODO: De loop van Arduino, deze blijft telkens herhalen
//       Hier kies je een type sensor:
//           - perdiodiek uitgelezen (met delay erin)
//           - event gebasseerde sensoren (zonder delay erin)
//**********************************************************
void loop()
{
    // Periodiek sensor uitlezen
    getSensorValue();
    // OF
    // Event gebasseerde sensor, blocking - delay in commentaar zetten!
    // waitForEvent();

    // Verzenden met LoRa
    sendWithLoRa();

    // Delay verwijderen bij event-gebasseerde sensoren
    delay(10000);     // Tijd om te wachten (milliseconden)
}

//**********************************************************
// TODO: Uitlezen van een periodieke sensor en vullen van buffer.
//       Dit moet worden aangepast naargelang de sensor
//**********************************************************
void getSensorValue()
{

  int visible = SI1145.ReadVisible();
  int infrared = SI1145.ReadIR();
  int ultraviolet = SI1145.ReadUV();
  
  debugSerial.print("//--------------------------------------//\r\n");
  debugSerial.print("Vis: "); debugSerial.println(visible);
  debugSerial.print("IR: "); debugSerial.println(infrared);
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  debugSerial.print("UV: ");  debugSerial.println((float)ultraviolet/100);



    // Buffer vullen met onze data (temperatuur)
    buffer[0] = ((int) visible >> 8) & 0xFF;
    buffer[1] = ((int) visible >> 0) & 0xFF;
    buffer[2] = ((int) infrared >> 8) & 0xFF;
    buffer[3] = ((int) infrared >> 0) & 0xFF;
    buffer[4] = ((int) ultraviolet >> 8) & 0xFF;
    buffer[5] = ((int) ultraviolet >> 0) & 0xFF;
    numberOfDataBytes = 6;
}

//**********************************************************
// TODO: Wachten op een verandering voor een event gebasseerde
//       sensor.
//**********************************************************
void waitForEvent()
{
  // Lees de huidige stand van de drukknop
  int previousState = digitalRead(BUTTON);
  int state = previousState;

  debugSerial.println("Wachten voor event");

  // Wachten op verandering van de staat van de knop.
  // We wachten ook zolang de knop ingedrukt is (state == HIGH)
  //    (loslaten negeren we dus, enkel indrukken)
  while (state == previousState || state  == HIGH) {
    previousState = state;          // Nieuwe staat opslaan in oude staat
    state = digitalRead(BUTTON);    // Nieuwe staat inlezen
    delay(10);    // Even wachten voor ontdendering
  }

  debugSerial.println("Event is gebeurt");

  // Opslaan in buffer om te verzenden
  buffer[0] = HIGH;
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
