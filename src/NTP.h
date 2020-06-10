/*
 * NTP.h
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

/** @file NTP/NTP.h
 *  @brief NTP client library for Particle IoT devices.
 *		   Please read 
 *			- https://tools.ietf.org/html/rfc1305
 *		   when you need more information.
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <stdio.h>
#include <application.h>

class NTP
{
    public:
        /** @brief Default NTP port.
         */
        #define NTP_DEFAULT_PORT            123

        /** @brief Default update time in seconds.
         */
        #define NTP_DEFAULT_TIME            60

        /** @brief Timeout time in seconds
         */
        #define NTP_DEFAULT_TIMEOUT         10

        /** @brief NTP protocol version.
         */
        #define NTP_VERSION                 3
        
        /** @brief NTP error codes.
         */
        typedef enum
        {
            NO_ERROR = 0x00,							        /**< No error. */
            INVALID_TIME = 0x01,						        /**< Invalid time. */
            INVALID_PARAMETER = 0x01,							/**< Invalid function parameter. */
            TRANSMISSION_ERROR = 0x02,							/**< General transmission error. */
            TIMEOUT = 0x03,							            /**< Timeout while connecting with server. You have to restart the NTP client. */
            WAIT = 0x04,                                        /**< Wait for next update intervall. */ 
        } Error;

        /** @brief NTP mode codes.
         */
        typedef enum
        {
            SYM_ACTIVE = 0x01,                                  /**< Symmetric active mode. */
            SYM_PASSIVE = 0x02,                                 /**< Symmetric passive mode. */
            CLIENT = 0x03,                                      /**< Client mode. */
            SERVER = 0x04,                                      /**< Server mode. */
            BROAD = 0x05,                                       /**< Broadcast or multicast mode. */
            CONTROL = 0x06,                                     /**< NTP control message. */
        } Mode;

        /** @brief NTP leap indicator codes.
         */
        typedef enum
        {
            NO_WARNING = 0x00,                                  /**< No warning. */
            LONG_MINUTE = 0x01,                                 /**< Last minute has 61 seconds. */
            SHORT_MINUTE = 0x02,                                /**< Last minute has 59 seconds. */
            ALERT = 0x03,                                       /**< Alarm condition (clock not synchronized). */
        } Leap;

        /** @brief NTP message packet.
         */
        typedef struct
        {
            uint8_t Mode:3;                                     /**< This is an integer indicating the association mode, with values coded as follows: \n
                                                                        0x00    Unspecified \n
                                                                        0x01    Symetric active \n
                                                                        0x02    Symetric passive \n
                                                                        0x03    Client \n
                                                                        0x04    Server \n
                                                                        0x05    Broadcast */
            uint8_t VN:3; 						                /**< Version number. */
            uint8_t LI:2; 					                    /**< This is a two-bit code warning of an impending leap second to be inserted in the NTP timescale. \n
                                                                     The bits are set before 23:59 on the day of insertion and reset after 00:00 on the following day. \n
                                                                     This causes the number of seconds (rollover interval) in the day of insertion to be increased \n 
                                                                     or decreased by one. In the case of primary servers the bits are set by operator intervention, \n
                                                                     while in the case of secondary servers the bits are set by the protocol. \n
                                                                     The two bits, bit 0 and bit 1, respectively, are coded as follows: \n
                                                                        0x00    No warning \n
                                                                        0x01    Last minute has 61 seconds \n
                                                                        0x02    Last minute has 59 seconds \n
                                                                        0x03    Alarm condition (clock not synchronized) */
            uint8_t Strat; 							            /**< This is an integer indicating the stratum of the local clock, with values defined as follows: \n
                                                                        0x00            Unspecified \n
                                                                        0x01            Primary reference (e. g. calibrated atomic clock) \n
                                                                        0x02 - 0xFF     Secondary reference (via NTP) */
            int8_t Poll;							            /**< This is a signed integer indicating the minimum interval between transmitted messages, \n
                                                                     in seconds as a power of two. For instance, a value of six indicates a minimum interval \n
                                                                     of 64 seconds. */
            int8_t Prec;							            /**< This is a signed integer indicating the precision of the various clocks, in seconds to the \n
                                                                     nearest power of two. The value must be rounded to the next larger power of two; for instance, \n
                                                                     a 50-Hz (20 ms) or 60-Hz (16.67 ms) power-frequency clock would be assigned the value -5 (31.25 ms), \n
                                                                     while a 1000-Hz (1 ms) crystal-controlled clock would be assigned the value -9 (1.95 ms). */
            int32_t RootDelay;                                  /**< This is a signed fixed-point number indicating the total roundtrip delay to the primary reference \n
                                                                     source at the root of the synchronization subnet, in seconds. Note that this variable can take on both \n
                                                                     positive and negative values, depending on clock precision and skew. */
            int32_t RootDispersion;							    /**< This is a signed fixed-point number indicating the maximum error relative to the primary reference \n
                                                                     source at the root of the synchronization subnet, in seconds. Only positive values greater than zero are possible. \n */
            uint32_t RefIdentifier;							    /**< This is a 32-bit code identifying the particular reference clock. In the case of stratum 0 (unspecified) or stratum 1 \n
                                                                     (primary reference source), this is a four-octet, left-justified, zero-padded ASCII string. */
            uint32_t RefTimestamp_S;							/**< This is the local time, in timestamp format, when the local clock was last updated. If the local clock has never been \n
                                                                     synchronized, the value is zero. */
            uint32_t RefTimestamp_F;							/**< Reference time-stamp fraction of a second. */
            uint32_t OriginateTimestamp_S;						/**< This is the local time, in timestamp format, at the peer when its latest NTP message was sent. If the peer becomes \n
                                                                     unreachable the value is set to zero. */
            uint32_t OriginateTimestamp_F;						/**< Originate time-stamp fraction of a second. */
            uint32_t ReceiveTimestamp_S;    					/**< This is the local time, in timestamp format, when the latest NTP message from the peer arrived. If the peer becomes \n
                                                                     unreachable the value is set to zero. */
            uint32_t ReceiveTimestamp_F;    					/**< Received time-stamp fraction of a second. */
            uint32_t TransmitTimestamp_S;		    			/**< This is the local time, in timestamp format, at which the NTP message departed the sender. */
            uint32_t TransmitTimestamp_F;		    			/**< Transmit time-stamp fraction of a second. */
        } __attribute__((packed)) NTP_Packet;

        /** @brief          Constructor.
         *  @param Server   NTP server address
         */
        NTP(const char* Server);

        /** @brief              Constructor.
         *  @param Server       NTP server address
         *  @param Port         Port used by the NTP server
         */
        NTP(const char* Server, uint16_t Port);

        /** @brief              Constructor.
         *  @param Server       NTP server address
         *  @param Port         Port used by the NTP server
         *  @param UpdateTime   Update time in seconds
         */
        NTP(const char* Server, uint16_t Port, uint16_t Update);

        /** @brief              Constructor.
         *  @param Server       NTP server address
         *  @param Port         Port used by the NTP server
         *  @param Timeout      Timeout time in seconds
         *  @param UpdateTime   Update time in seconds
         */     
        NTP(const char* Server, uint16_t Port, uint16_t Update, uint16_t Timeout);

        /** @brief Deconstructor.
         */
        ~NTP();

        /** @brief          Set the timeout time for the NTP client.
         *  @param Timeout  Timeout time in seconds
         */ 
        void SetTimeout(uint16_t Timeout);

        /** @brief              Set the update time for the NTP client.
         *  @param UpdateTime   Update time in seconds
         */ 
        void SetUpdateTime(uint16_t UpdateTime);

        /** @brief          Get a new timestamp from the NTP server.
         *  @param Seconds  Pointer to time since 01/01/1970
         *  @param Millis   Pointer to milliseconds
         *  @param Leap     Leap information from the server
         *  @return         Error code
         */
        NTP::Error Update(uint32_t* Seconds, uint32_t* Millis, NTP::Leap* Leap);

        /** @brief          Force a new timestamp from the NTP server.
         *  @param Seconds  Pointer to time since 01/01/1970
         *  @param Millis   Pointer to milliseconds
         *  @param Leap     Leap information from the server
         *  @return         Error code
         */
        NTP::Error ForceUpdate(uint32_t* Seconds, uint32_t* Millis, NTP::Leap* Leap);

    private:
        const char* _mServer;

        UDP _mClient;

        NTP::NTP_Packet _mPacket;

        uint16_t _mPort;

        uint32_t _mTimeout;
        uint32_t _mSendTime;
        uint32_t _mRecTime;
        uint32_t _mUpdateTime;
        uint32_t _mLastUpdate;

        /** @brief              Load all neccessary variables.
         *  @param Server       NTP server address
         *  @param Port         Port used by the NTP client
         *  @param UpdateTime   Update time in seconds
         *  @param Timeout      Timeout time in seconds
         */
        void _init(const char* Server, uint16_t Port, uint16_t UpdateTime, uint16_t Timeout);

        /** @brief          Calculate the power of a given value.
         *  @param Base     Base
         *  @param Exp      Exponent
         */
        uint32_t _power(uint8_t Base, uint8_t Exp);
};