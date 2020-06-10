/*
 * Client.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: NTP version 3 client example for Particle IoT devices.

  GNU GENERAL PUBLIC LICENSE:
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Errors and omissions should be reported to DanielKampert@kampis-elektroecke.de
 */

#include <NTP.h>

NTP Server("pool.ntp.org");

uint32_t Seconds;
uint32_t Millis;
NTP::Leap Leap;

void setup() 
{
    Serial.begin(9600);
    Serial.println("--- NTP example ---");
}

void loop()
{
    NTP::Error Error = Server.Update(&Seconds, &Millis, &Leap);
    if(Error == NTP::NO_ERROR)
    {
        Serial.printlnf("Time: %u,%u", Seconds, Millis);
        Serial.printlnf("Leap: %u", Leap);
    }
    else if(Error == NTP::WAIT)
    {
        Serial.println("Wait for update...");
    }
    else
    {
        Serial.println("Error! Retry...");
    }

    delay(1000);
}