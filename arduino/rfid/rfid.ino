const int SIZE_OF_BUFFER = 32;
uint8_t buffer[SIZE_OF_BUFFER];

void setup() {
    // put your setup code here, to run once:
    SerialUSB.begin(115200);
    while ((!SerialUSB) && (millis() < 5000));
    SerialUSB.println("Starten van RFID demo");
    Serial.begin(9600);         // Serial is verbonden met de RFID lezer
}
 
void loop()
{
    // Lees het id van een RFID kaart
    read_rfid_card();   // Blocking
    
    SerialUSB.print("Kaart gedetecteerd met ID = ");
    SerialUSB.println((char*)buffer);
}

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
