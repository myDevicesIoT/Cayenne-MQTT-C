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

#if !defined(__NETWORK_h)
#define __NETWORK_h

#if defined(WIN32_DLL) || defined(WIN64_DLL)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#elif defined(LINUX_SO)
  #define DLLImport extern
  #define DLLExport  __attribute__ ((visibility ("default")))
#else
  #define DLLImport
  #define DLLExport
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

/**
* Network struct for reading from and writing to a network connection.
*/
typedef struct Network
{
	SOCKET my_socket;  /**< The connection socket. */
	int connected;  /**< Connection state, 1 if connected, 0 if not. */
	
	/**
	* Read data from the network.
	* @param[in] network Pointer to the Network struct
	* @param[out] buffer Buffer that receives the data
	* @param[in] len Buffer length
	* @param[in] timeout_ms Timeout for the read operation, in milliseconds
	* @return Number of bytes read, or a negative value if there was an error
	*/
	int(*mqttread) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
	
	/**
	* Write data to the network.
	* @param[in] network Pointer to the Network struct
	* @param[in] buffer Buffer that contains data to write
	* @param[in] len Number of bytes to write
	* @param[in] timeout_ms Timeout for the write operation, in milliseconds
	* @return Number of bytes written, or a negative value if there was an error
	*/
	int(*mqttwrite) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
} Network;

/**
* Read data from the network.
* @param[in] network Pointer to the Network struct
* @param[out] buffer Buffer that receives the data
* @param[in] len Buffer length
* @param[in] timeout_ms Timeout for the read operation, in milliseconds
* @return Number of bytes read, or a negative value if there was an error
*/
int NetworkRead(struct Network* network, unsigned char* buffer, int len, int timeout_ms);

/**
* Write data to the network.
* @param[in] network Pointer to the Network struct
* @param[in] buffer Buffer that contains data to write
* @param[in] len Number of bytes to write
* @param[in] timeout_ms Timeout for the write operation, in milliseconds
* @return Number of bytes written, or a negative value if there was an error
*/
int NetworkWrite(struct Network* network, unsigned char* buffer, int len, int timeout_ms);

/**
* Initialize Network struct
* @param[in] network Pointer to the Network struct
*/
DLLExport void NetworkInit(Network* network);

/**
* Connect to the specified address.
* @param[in] network Pointer to the Network struct
* @param[in] addr Destination address
* @param[in] port Destination port
* @return 0 if successfully connected, an error code otherwise
*/
DLLExport int NetworkConnect(Network* network, char* addr, int port);

/**
* Close the connection.
* @param[in] network Pointer to the Network struct
*/
DLLExport void NetworkDisconnect(Network* network);

/**
* Get the connection state.
* @param[in] network Pointer to the Network struct
* @return 1 if connected, 0 if not
*/
DLLExport int NetworkConnected(Network* network);

/**
* Cleanup the connection.
*/
DLLExport void NetworkCleanup();

#endif
