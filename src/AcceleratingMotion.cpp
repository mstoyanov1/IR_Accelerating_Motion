#include "AcceleratingMotion.h"

AcceleratingMotion::AcceleratingMotion(const int* pinArray, int sz) {
    if (sz < 2) {
	Serial.println("[AcceleratingMotion]: At least 2 pins expected for reading. Make sure your setup is connected properly. Exiting...");
        exit(1);
    } else {
	size = sz;
    }

    // allocate memory for the pins given and the delta readings
    pins = (int *) malloc(size * sizeof(int));
    deltas = (unsigned long *) malloc((size-1) * sizeof(unsigned long));

    // notify user upon memory allocation failure and exit
    if (pins == NULL || deltas == NULL) {
	Serial.println("[AcceleratingMotion]: Not enough memory to initialize. Please free some memory on your device and try again. Exiting...");
        exit(1);
    }

    // map given pins to internal array
    for (int i = 0; i < size; i++) {
      pins[i] = *pinArray++;
    }

    // (re-)set/init control variable and time variables
    reset();
}

void AcceleratingMotion::readPins() {
    for (int i = 0, j = -1; i < size; i++, j++) {
      int pin = pins[i];                            // currently tested pin
      if (LOW == digitalRead(pin) && next == pin) { // react only if IR sensor connection broken (object detected) and this is the currently expected pin
        if (pin == pins[0]) {                       // is this the first pin?
          time_in = millis();                       // setup reference time
          msgStart();                               // inform the user a new measurement has started
        } else {
	  unsigned long current_time = millis();
	  if (current_time < time_in) {             // time overflow? -> reset the experiment
	    Serial.println("Overflow...Resetting...");
	    reset();
	    break;
	  }
          deltas[j] =  current_time - time_in;      // measure current delta from reference time
          displayReading("t", i, deltas[j]);        // display the current reading on display & serial monitor
          if (pin == pins[size-1]) {                // is this the last pin?
            msgEnd();                               // inform the user that the measurement has ended
            reset();                                // once last pin becomes active it is time to reset the system to initial state
            break;                                  // no need to set next, since part of reset() and PINS[i+1] would be out of the array bounds
          }
       }
       next = pins[i+1];                            // set next expected active pin
       break;                                       // and break, since the active pin has been detected - no need to check the rest (if any) within the current loop iteration - those are not allowed to be active
      }
    }
}

void AcceleratingMotion::begin() {
  // initialize all pins as input pins and setup the oled display
  for (int i = 0; i < size; i++) {
    pinMode(pins[i], INPUT);
  }
  setupDisplay();
}

unsigned long AcceleratingMotion::getDelta(int index) {
  if (index < 0 || index > size) return 0; // silent skip out of bounds issues with time readings
  return deltas[index];
}

void AcceleratingMotion::setupDisplay() {
    Wire.begin();                 // Needed for the led display
    SeeedOled.init();             // Initialze SEEED OLED display
    SeeedOled.clearDisplay();     // Clear the screen and set start position to top left corner
    SeeedOled.setNormalDisplay(); // Set display to normal mode (i.e non-inverse mode)
    SeeedOled.setPageMode();      // Set addressing mode to Page Mode

    SeeedOled.setTextXY(size,0);
    SeeedOled.putString("Roll the ball!");
}

void AcceleratingMotion::msgStart() {
    Serial.println("New measurement started...");
    SeeedOled.clearDisplay();
    SeeedOled.putString("Start...");
}

void AcceleratingMotion::msgEnd() {
    SeeedOled.setTextXY(size,0); SeeedOled.putString("End...");
    SeeedOled.setTextXY(0,0); SeeedOled.putString("Roll the ball!");
}

void AcceleratingMotion::displayReading(const char * paramName, int index, unsigned long value) {
    char buf[30];
    snprintf(buf, 30, "%s%d: %lu ms", paramName, index, value);

    Serial.println(buf);
    
    SeeedOled.setTextXY(index, 0);
    SeeedOled.putString(buf);
}

void AcceleratingMotion::displayPinStats() {
    char buf[24];
    for (int i = 0; i < size; i++) {
      SeeedOled.setTextXY(i, 0);
      snprintf(buf, 24, "PIN %d: %s", pins[i], LOW == digitalRead(pins[i]) ? "LOW" : "HIGH");
      SeeedOled.putString(buf);
    }
}

void AcceleratingMotion::reset() {
    // set next pin expected to the front of the pin list
    next = pins[0];
    // zero time variables
    time_in = 0;
    memset(deltas, 0, (size-1) * sizeof(*deltas));
}

// @todo remove me for production enviroments
void AcceleratingMotion::debug() {
    char buf[24];
    for (int i = 0; i < size; i++) {
      snprintf(buf, 24, "PIN %d: %s", pins[i], LOW == digitalRead(pins[i]) ? "LOW" : "HIGH");
      Serial.println(buf);
    }
}

