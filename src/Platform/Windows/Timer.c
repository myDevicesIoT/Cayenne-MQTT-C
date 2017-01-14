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

#include "Timer.h"

void TimerInit(Timer* timer)
{
	timer->endTime.QuadPart = 0;
	QueryPerformanceFrequency(&timer->countsPerMS);
	timer->countsPerMS.QuadPart /= 1000;
}

char TimerIsExpired(Timer* timer)
{
	LARGE_INTEGER now, difference;
	QueryPerformanceCounter(&now);
	difference.QuadPart = timer->endTime.QuadPart - now.QuadPart;
	return difference.QuadPart < 0;
}


void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	LARGE_INTEGER add;
	add.QuadPart = timeout;
	add.QuadPart *= timer->countsPerMS.QuadPart;
	timer->endTime.QuadPart = now.QuadPart + add.QuadPart;
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	LARGE_INTEGER add;
	add.QuadPart = timeout * 1000;
	add.QuadPart *= timer->countsPerMS.QuadPart;
	timer->endTime.QuadPart = now.QuadPart + add.QuadPart;
}


int TimerLeftMS(Timer* timer)
{
	LARGE_INTEGER now, difference;
	QueryPerformanceCounter(&now);

	difference.QuadPart = timer->endTime.QuadPart - now.QuadPart;
	difference.QuadPart /= timer->countsPerMS.QuadPart;
	return difference.LowPart;
}
