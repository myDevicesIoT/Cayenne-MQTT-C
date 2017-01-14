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

#include "Network.h"


int NetworkRead(Network* network, unsigned char* buffer, int len, int timeout_ms)
{
	// Make sure the timeout isn't zero, otherwise it can block forever.
	DWORD interval = 1;
	setsockopt(network->my_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&interval, sizeof(interval));

	int bytes = 0;
	while (bytes < len && network->connected)
	{
		int rc = recv(network->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		if (rc == -1)
		{
			if (errno != ENOTCONN && errno != ECONNRESET)
			{
				bytes = -1;
				break;
			}
		}
		else if ((rc == 0 && len != 0) || errno == ENOTCONN || errno == ECONNRESET || errno == EPIPE)
			network->connected = 0;
		else
			bytes += rc;
	}
		
	return bytes;
}


int NetworkWrite(Network* network, unsigned char* buffer, int len, int timeout_ms)
{
	DWORD interval = 1;
	setsockopt(network->my_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&interval, sizeof(interval));
	int	rc = send(network->my_socket, buffer, len, 0);
	if (rc == -1 && (errno == ENOTCONN || errno == ECONNRESET || errno == EPIPE))
		network->connected = 0;

	return rc;
}


void NetworkInit(Network* network)
{
	network->my_socket = 0;
	network->connected = 0;
	network->mqttread = NetworkRead;
	network->mqttwrite = NetworkWrite;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}


int NetworkConnect(Network* network, char* addr, int port)
{
	int type = SOCK_STREAM;
	struct sockaddr_in address;
	int rc = -1;
	ADDRESS_FAMILY family = AF_INET;
	struct addrinfo *result = NULL;
	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
	{
		struct addrinfo* res = result;

		/* prefer ip4 addresses */
		while (res)
		{
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}

		if (result->ai_family == AF_INET)
		{
			address.sin_port = htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else
			rc = -1;

		freeaddrinfo(result);
	}

	if (rc == 0)
	{
		network->my_socket = socket(family, type, 0);
		if (network->my_socket != -1) {
			if ((rc = connect(network->my_socket, (struct sockaddr*)&address, sizeof(address))) == 0)
				network->connected = 1;
		}
	}

	return rc;
}


void NetworkDisconnect(Network* network)
{
	closesocket(network->my_socket);
	network->connected = 0;
}


int NetworkConnected(Network* network)
{
	return network->connected;
}

void NetworkCleanup()
{
	WSACleanup();
}