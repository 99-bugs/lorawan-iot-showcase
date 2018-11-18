#include "Ultrasonic.h"

#define debugSerial SerialUSB
#define SONIC_PIN 15
#define MEASURE_DELAY 500

Ultrasonic ultrasonic(SONIC_PIN);

void setup() {
    debugSerial.begin(115200);
    while ((!debugSerial) && (millis() < 5000));
    debugSerial.println("Starting Ultrasonic Ranger Demo");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    debugSerial.println("The distance to obstacles in front is: ");
    long range = ultrasonic.MeasureInCentimeters();    // two measurements should keep an interval
    debugSerial.print(range);  //0~400cm
    debugSerial.println(" cm");

    int timesBlink = (MEASURE_DELAY) / (10*range);    // Small range, fast blink, x10 to slow down a bit
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
}