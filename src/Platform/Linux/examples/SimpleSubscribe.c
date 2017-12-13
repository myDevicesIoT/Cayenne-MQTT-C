/**
* @file SimpleSubscribe.c
*
* Simplified example app for using the Cayenne MQTT C library to receive example data.
*/

#include "CayenneMQTTClient.h"

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char* username = "MQTT_USERNAME";
char* password = "MQTT_PASSWORD";
char* clientID = "CLIENT_ID";

Network network;
CayenneMQTTClient mqttClient;


// Handle messages received from the Cayenne server.
void messageArrived(CayenneMessageData* message)
{
	printf("Message received on channel %d\n", message->channel);
	
	// Add code to process the message here.
	if (message->topic == COMMAND_TOPIC) {
		// If this is a command message we publish a response to show we recieved it. Here we are just sending a default 'OK' response.
		// An error response should be sent if there are issues processing the message.
		CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, NULL);

		// Send the updated state for the channel so it is reflected in the Cayenne dashboard. If a command is successfully processed
		// the updated state will usually just be the value received in the command message.
		CayenneMQTTPublishData(&mqttClient, message->clientID, DATA_TOPIC, message->channel, NULL, NULL, message->values[0].value);
	}
}

// Connect to the Cayenne server.
int connectClient(void)
{
	// Connect to the server.
	int error = 0;
	printf("Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
	if ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
		return error;
	}

	if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {
		NetworkDisconnect(&network);
		return error;
	}
	printf("Connected\n");

	// Subscribe to required topics. Here we subscribe to the Command and Config topics.
	CayenneMQTTSubscribe(&mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
	CayenneMQTTSubscribe(&mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL);

	return CAYENNE_SUCCESS;
}


// Main loop where MQTT code is run.
void loop(void)
{
	while (1) {
		// Yield to allow MQTT message processing.
		CayenneMQTTYield(&mqttClient, 1000);
	}
}

// Main function.
int main(int argc, char** argv)
{
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

	return 0;
}


