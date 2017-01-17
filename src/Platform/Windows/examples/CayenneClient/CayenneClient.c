/**
* @file CayenneClient.c
*
* Example app for using the Cayenne MQTT C library to send and receive example data.
*/

#include <stdio.h>
#include <stdbool.h>
#include "CayenneMQTTClient.h"
#include "MQTTWindows.h"

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char* username = "MQTT_USERNAME";
char* password = "MQTT_PASSWORD";
char* clientID = "WIN_CLIENT_ID";

Network network;
CayenneMQTTClient mqttClient;
bool finished = false;


/**
* Print the message info.
* @param[in] message The message received from the Cayenne server.
*/
void outputMessage(CayenneMessageData* message)
{
	size_t i;
	switch (message->topic) {
	case COMMAND_TOPIC:
		printf("topic=Command");
		break;
	case CONFIG_TOPIC:
		printf("topic=Config");
		break;
	default:
		printf("topic=%d", message->topic);
		break;
	}
	printf(" channel=%d", message->channel);
	if (message->clientID) {
		printf(" clientID=%s", message->clientID);
	}
	if (message->type) {
		printf(" type=%s", message->type);
	}
	for (i = 0; i < message->valueCount; ++i) {
		if (message->values[i].value) {
			printf(" value=%s", message->values[i].value);
		}
		if (message->values[i].unit) {
			printf(" unit=%s", message->values[i].unit);
		}
	}
	if (message->id) {
		printf(" id=%s", message->id);
	}
	printf("\n");
}

/**
* Handle messages received from the Cayenne server.
* @param[in] message The message received from the Cayenne server.
*/
void messageArrived(CayenneMessageData* message)
{
	int error = 0;
	// Add code to process the message. Here we just ouput the message data.
	outputMessage(message);

	if (message->topic == COMMAND_TOPIC) {
		// If this is a command message we publish a response to show we recieved it. Here we are just sending a default 'OK' response.
		// An error response should be sent if there are issues processing the message.
		if ((error = CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, NULL)) != CAYENNE_SUCCESS) {
			printf("Response failure, error: %d\n", error);
		}

		// Send the updated state for the channel so it is reflected in the Cayenne dashboard. If a command is successfully processed
		// the updated state will usually just be the value received in the command message.
		if ((error = CayenneMQTTPublishData(&mqttClient, message->clientID, DATA_TOPIC, message->channel, NULL, NULL, message->values[0].value)) != CAYENNE_SUCCESS) {
			printf("Publish state failure, error: %d\n", error);
		}
	}
}

/**
* Connect to the Cayenne server.
* @return Returns CAYENNE_SUCCESS if the connection succeeds, or an error code otherwise.
*/
int connectClient(void)
{
	int error = 0;
	// Connect to the server.
	printf("Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
	while ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
		if (finished)
			return error;
		printf("TCP connect failed, error: %d\n", error);
		Sleep(2);
	}

	if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {
		printf("MQTT connect failed, error: %d\n", error);
		return error;
	}
	printf("Connected\n");

	// Subscribe to required topics.
	if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
		printf("Subscription to Command topic failed, error: %d\n", error);
	}
	if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
		printf("Subscription to Config topic failed, error:%d\n", error);
	}

	// Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
	CayenneMQTTPublishData(&mqttClient, NULL, SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
	CayenneMQTTPublishData(&mqttClient, NULL, SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "Windows");

	return CAYENNE_SUCCESS;
}

/**
* Main loop where MQTT code is run.
*/
void loop(void)
{
	Timer timer;
	TimerInit(&timer);
	//Start the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
	TimerCountdown(&timer, 5);

	while (!finished) {
		// Yield to allow MQTT message processing.
		CayenneMQTTYield(&mqttClient, 1000);

		// Check that we are still connected, if not, reconnect.
		if (!NetworkConnected(&network) || !CayenneMQTTConnected(&mqttClient)) {
			NetworkDisconnect(&network);
			CayenneMQTTDisconnect(&mqttClient);
			printf("Reconnecting\n");
			while (connectClient() != CAYENNE_SUCCESS) {
				if (finished)
					return;
				Sleep(2);
				printf("Reconnect failed, retrying\n");
			}
		}

		// Publish some example data every few seconds. This should be changed to send your actual data to Cayenne.
		if (TimerIsExpired(&timer)) {
			int error = 0;
			if ((error = CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5)) != CAYENNE_SUCCESS) {
				printf("Publish temperature failed, error: %d\n", error);
			}
			if ((error = CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000)) != CAYENNE_SUCCESS) {
				printf("Publish luminosity failed, error: %d\n", error);
			}
			if ((error = CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 2, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, 800)) != CAYENNE_SUCCESS) {
				printf("Publish barometric pressure failed, error: %d\n", error);
			}
			//Restart the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
			TimerCountdown(&timer, 5);
		}
	}
}

/**
* Control signal handler for processing program shutdown.
* @param[in] dwCtrlType The type of control signal received by the handler.
* @return TRUE if the control signal was handled.
*/
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	BOOL handled = FALSE;
	if (dwCtrlType == CTRL_C_EVENT) {
		finished = true;
		handled = TRUE;
	}
	return handled;
}

/**
* Main function.
* @param[in] argc Count of command line arguments.
* @param[in] argv Command line argument string array.
*/
int main(int argc, char** argv)
{
	// Set up handler so we can exit cleanly when Ctrl-C is pressed.
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	// Initialize the network.
	NetworkInit(&network);

	// Initialize the Cayenne client.
	CayenneMQTTClientInit(&mqttClient, &network, username, password, clientID, messageArrived);

	// Connect to Cayenne.
	if (connectClient() == CAYENNE_SUCCESS) {
		// Run main loop.
		loop();
	}
	else {
		printf("Connection failed, exiting\n");
	}

	// Disconnect from Cayenne.
	if (CayenneMQTTConnected(&mqttClient))
		CayenneMQTTDisconnect(&mqttClient);

	// Disconnect network client.
	if (NetworkConnected(&network))
		NetworkDisconnect(&network);

	// Cleanup the network.
	NetworkCleanup();

	return 0;
}


