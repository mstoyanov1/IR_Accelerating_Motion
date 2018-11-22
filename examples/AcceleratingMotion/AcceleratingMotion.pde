#include <AcceleratingMotion.h>

int PINS[] = {2,3,4,5};
AcceleratingMotion motion(PINS, 4);

void setup() {
  Serial.begin(9600);
  motion.begin();
  motion.displayPinStats();
}

void loop() {
  motion.debug();
  motion.readPins();  
}
