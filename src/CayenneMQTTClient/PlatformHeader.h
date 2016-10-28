/*
The MIT License(MIT)

Cayenne MQTT Client Library
Copyright (c) 2016 myDevices

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PLATFORMHEADER_H_
#define PLATFORMHEADER_H_

//Set the platform specific header file containing the Network and Timer definitions here.
#if defined(__linux__)
	#include "MQTTLinux.h"
#endif

// The following Network and Timer code should be defined in the platform specific header.
//
// Interface for the platform specific Network struct used by MQTTClient. Your platform specific Network struct must provide the same functions.
//
// typedef struct Network
// {
//     /**
//     * Read data from the network.
//     * @param[in] network Pointer to the Network struct
//     * @param[out] buffer Buffer that receives the data
//     * @param[in] len Buffer length
//     * @param[in] timeout_ms Timeout for the read operation, in milliseconds
//     * @return 1 for success, any other value for error
//     */
//     int(*mqttread) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
//
//     /**
//     * Write data to the network.
//     * @param[in] network Pointer to the Network struct
//     * @param[in] buffer Buffer that contains data to write
//     * @param[in] len Number of bytes to write
//     * @param[in] timeout_ms Timeout for the write operation, in milliseconds
//     * @return Number of bytes written on success, a negative value for error
//     */
//     int(*mqttwrite) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
// } Network;
//
//
// A Timer struct should be defined in the platform specific header.
//
// typedef struct Timer
// {
// } Timer;
//
// The following functions should be provided for using the Timer.
//
// /**
// * Initialize countdown timer
// * @param[in] timer Pointer to Timer struct
// */
// void TimerInit(Timer* timer);
//
// /**
// * The countdown timer has expired.
// * @param[in] timer Pointer to Timer struct
// * @return 1 if countdown has expired, 0 otherwise.
// */
// char TimerIsExpired(Timer* timer);
//
// /**
// * Start countdown in milliseconds.
// * @param[in] timer Pointer to Timer struct
// * @param[in] timeout Number of milliseconds to count down.
// */
// void TimerCountdownMS(Timer* timer, unsigned int timeout);
//
// /**
// * Start countdown in seconds.
// * @param[in] timer Pointer to Timer struct
// * @param[in] timeout Number of seconds to count down.
// */
// void TimerCountdown(Timer* timer, unsigned int timeout);
//
// /**
// * Get the number of milliseconds left in countdown.
// * @param[in] timer Pointer to Timer struct
// * @return Number of milliseconds left.
// */
// int TimerLeftMS(Timer* timer);


#define MAX_MESSAGE_HANDLERS 0  // Set MQTTClient handlers to 0 since Cayenne uses its own handlers.

#endif /* PLATFORMHEADER_H_ */
