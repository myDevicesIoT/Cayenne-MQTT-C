/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(__TIMER_h)
#define __TIMER_h

#include <sys/time.h>
#include <stdio.h>

 /**
 * Countdown timer struct.
 */
typedef struct Timer
{
	struct timeval end_time; /**< Countdown end time. */
} Timer;

/**
* Initialize countdown timer.
* @param[in] timer Pointer to Timer struct
*/
void TimerInit(Timer* timer);

/**
* The countdown timer has expired.
* @param[in] timer Pointer to Timer struct
* @return 1 if countdown has expired, 0 otherwise.
*/
char TimerIsExpired(Timer* timer);

/**
* Start countdown in milliseconds.
* @param[in] timer Pointer to Timer struct
* @param[in] timeout Number of milliseconds to count down.
*/
void TimerCountdownMS(Timer* timer, unsigned int timeout);

/**
* Start countdown in seconds.
* @param[in] timer Pointer to Timer struct
* @param[in] timeout Number of seconds to count down.
*/
void TimerCountdown(Timer* timer, unsigned int timeout);

/**
* Get the number of milliseconds left in countdown.
* @param[in] timer Pointer to Timer struct
* @return Number of milliseconds left.
*/
int TimerLeftMS(Timer* timer);

#endif
