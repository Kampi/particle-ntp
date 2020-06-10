#include "NTP/NTP.h"

NTP Server("pool.ntp.org");

uint32_t Seconds;
uint32_t Millis;
NTP::Leap Leap;

void setup() 
{
    Serial1.begin(9600);
}

void loop()
{
    NTP::Error Error = Server.Update(&Seconds, &Millis, &Leap);
    if(Error == NTP::NO_ERROR)
    {
        Serial1.printlnf("Time: %u,%u", Seconds, Millis);
        Serial1.printlnf("Leap: %u", Leap);
    }
    else if(Error == NTP::WAIT)
    {
        Serial1.println("Wait for update...");
    }
    else
    {
        Serial1.println("Error! Retry...");
    }

    delay(1000);
}