/*
  Accelerating Motion library v1.0
  2016 Copyright (c) Mihail Stoyanov.  All right reserved.

  Library currently has a dependency on an OLED display from SEEED.
  If you don't need it for your setup - please remove it from the code
  (removal is trivial) or send your inquiry via E-Mail at
  stoyanov.mihail@gmail.com

  Possible improvements:
   - remove display dependency
   - improved input validation
   - ability to export measured deltas for further processing
   - add time formatting support
   - time counter overflow detection
   - performance/space optimizations
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef __ACCELERATING_MOTION_H__
#define __ACCELERATING_MOTION_H__

#include <Wire.h>
#include <SeeedOLED.h>

class AcceleratingMotion
{
  public:
    AcceleratingMotion(const int* pinArray, int sz);
    void begin();
    void readPins();
    unsigned long getDelta(int index);
    void displayPinStats();
    void debug();
  private:
    int  size;
    int* pins;
    int  next;
    unsigned long time_in;
    unsigned long* deltas;
    void setupDisplay();
    void msgStart();
    void msgEnd();
    void displayReading(const char * paramName, int index, unsigned long value);
    void reset();
};
#endif

