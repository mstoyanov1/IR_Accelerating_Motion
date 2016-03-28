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

    // init the deltas with zeros
    memset(deltas, 0, (sz-1) * sizeof(*pinArray));
    
    // init internal pins array and reference time, setup control variable
    for (int i = 0; i < size; i++) {
      pins[i] = *pinArray++;
    }
    next = pins[0];
    time_in = 0;
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
          deltas[j] =  millis() - time_in;          // measure current delta from reference time
          displayReading("t", i, deltas[j]);        // display the current reading on display & serial monitor
          if (pin == pins[size-1]) {                   // is this the last pin?
            msgEnd();                               // inform the user that the measurement has ended
            reset();                                // once last pin becomes active it is time to reset the system to initial state
            break;                                  // no need to set next, since part of reset() and PINS[i+1] would be out of the array bounds
          }
       }
       next = pins[i+1];                           // set next expected active pin
       break;                                      // and break, since the active pin has been detected - no need to check the rest (if any) within the current loop iteration - those are not allowed to be active
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

    SeeedOled.setTextXY(4, 0);
    SeeedOled.putString("Roll the ball!");
}

void AcceleratingMotion::msgStart() {
    Serial.println("New measurement started...");
    SeeedOled.clearDisplay();
    SeeedOled.putString("Start...");
}

void AcceleratingMotion::msgEnd() {
    SeeedOled.setTextXY(4,0); SeeedOled.putString("End...");
    SeeedOled.setTextXY(0,0); SeeedOled.putString("Roll the ball!");
}

void AcceleratingMotion::displayReading(const char * paramName, int index, unsigned long value) {
    Serial.print(paramName); Serial.print(index); Serial.print(": "); Serial.print(value); Serial.println(" ms"); // @todo remove me for production environments
    SeeedOled.setTextXY(index, 0);
    SeeedOled.putString(paramName); SeeedOled.putNumber(index); SeeedOled.putString(": "); SeeedOled.putNumber(value); SeeedOled.putString(" ms");
}

void AcceleratingMotion::displayPinStats() {
    for (int i = 0, n = size; i < n; i++) {
      SeeedOled.setTextXY(i, 0);
      SeeedOled.putString("PIN "); SeeedOled.putNumber(pins[i]); SeeedOled.putString(": "); SeeedOled.putString(LOW == digitalRead(pins[i]) ? "LOW" : "HIGH");
    }
}

void AcceleratingMotion::reset() {
    // set next pin expected to the front of the pin list
    next = pins[0];
    // zero time variables
    time_in = 0;
    for (int i = 0, n = size - 1; i < n; i++) {
      deltas[i] = 0;
    }
}

// @todo remove me for production enviroments
void AcceleratingMotion::debug() {
    for (int i = 0; i < size; i++) {
      Serial.print("PIN "); Serial.print(pins[i]); Serial.print(": "); Serial.println(LOW == digitalRead(pins[i]) ? "LOW" : "HIGH");
    }
}

