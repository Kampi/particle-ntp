/*
 * NTP.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: NTP client library for Particle IoT devices.

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

/** @file NTP/NTP.cpp
 *  @brief NTP client library for Particle IoT devices.
 *
 *  @author Daniel Kampert
 */

#include "NTP.h"

#define __SWAP32__(x)     ((uint32_t)((((x) & 0xFF000000) >> 0x18) | (((x) & 0x00FF0000) >> 0x08) | (((x) & 0x0000FF00) << 0x08) | (((x) & 0x000000FF) << 0x18)))

NTP::NTP(const char* Server)
{
    this->_init(Server, NTP_DEFAULT_PORT, NTP_DEFAULT_TIME, NTP_DEFAULT_TIMEOUT);
}

NTP::NTP(const char* Server, uint16_t Port)
{
    this->_init(Server, Port, NTP_DEFAULT_TIME, NTP_DEFAULT_TIMEOUT);
}

NTP::NTP(const char* Server, uint16_t Port, uint16_t Update)
{
    this->_init(Server, Port, Update, NTP_DEFAULT_TIMEOUT);
}

NTP::NTP(const char* Server, uint16_t Port, uint16_t Update, uint16_t Timeout)
{
    this->_init(Server, Port, Update, Timeout);
}

NTP::~NTP(void)
{
}

void NTP::SetUpdateTime(uint16_t UpdateTime)
{
    this->_mUpdateTime = UpdateTime;
}

void NTP::_init(const char* Server, uint16_t Port, uint16_t UpdateTime, uint16_t Timeout)
{
    this->_mServer = Server;
    this->_mPort = Port;
    this->_mUpdateTime = UpdateTime;
    this->_mTimeout = Timeout * 1000;
    this->_mLastUpdate = 0x00;
}

NTP::Error NTP::Update(uint32_t* Seconds, uint32_t* Millis, NTP::Leap* Leap)
{
    if((millis() - this->_mLastUpdate) > (this->_mUpdateTime * 1000))
    {
        return this->ForceUpdate(Seconds, Millis, Leap);
    }

    return WAIT;
}

NTP::Error NTP::ForceUpdate(uint32_t* Seconds, uint32_t* Millis, NTP::Leap* Leap)
{
    if((Seconds == NULL) || (Millis == NULL) || (Leap == NULL))
    {
        return INVALID_PARAMETER;
    }

    // Prepare the package
    memset(&this->_mPacket, 0x00, sizeof(NTP::NTP_Packet));
    this->_mPacket.VN = NTP_VERSION & 0x07;
    this->_mPacket.Mode = CLIENT;

    // Request a new package from the server
    this->_mClient.begin(this->_mPort);
    this->_mClient.beginPacket(this->_mServer, this->_mPort);
    this->_mClient.write((const uint8_t*)&this->_mPacket, sizeof(NTP::NTP_Packet));
    this->_mClient.endPacket();

    this->_mSendTime = millis();

    do
    {
        if(millis() > (this->_mSendTime + this->_mTimeout))
        {
            return TIMEOUT;
        }

        delay(10);
    }while(!this->_mClient.parsePacket());

    this->_mClient.read((unsigned char*)&this->_mPacket, sizeof(NTP::NTP_Packet));
    this->_mClient.stop();

    this->_mRecTime = millis();

    // Swap the endianess of the 32 bit fields
    for(uint8_t i = 0x01; i < (sizeof(NTP::NTP_Packet) / 0x04); i++)
    {
        *(((uint32_t*)(&this->_mPacket)) + i) = __SWAP32__(*(((uint32_t*)(&this->_mPacket)) + i));
    }

    if(this->_mPacket.Strat == 0x00)
    {
        return TRANSMISSION_ERROR;
    }

    uint32_t ServerPoll = this->_power(2, this->_mPacket.Poll);
    if(this->_mUpdateTime < ServerPoll)
    {
        this->_mUpdateTime = ServerPoll;
    }

    // Ignore the time before the 01/01/1970
    this->_mPacket.TransmitTimestamp_S -= 2208988800UL;

    // Get the milliseconds
    *Millis = (uint32_t)(((double)this->_mPacket.TransmitTimestamp_F) / 0xFFFFFFFF * 1000);

    // Add the routing delay for the communication with the server
    *Millis += this->_mRecTime - this->_mSendTime;
    if(*Millis >= 1000)
    {
        *Millis -= 1000;
        this->_mPacket.TransmitTimestamp_S++;
    }

    *Seconds = this->_mPacket.TransmitTimestamp_S;

    this->_mLastUpdate = millis();

    return NO_ERROR;
}

uint32_t NTP::_power(uint8_t Base, uint8_t Exp)
{
    uint32_t Result = 0x01;

    for(uint8_t i = 0x00; i < Exp; i++)
    {
        Result *= Base;
    }

    return Result;
 }