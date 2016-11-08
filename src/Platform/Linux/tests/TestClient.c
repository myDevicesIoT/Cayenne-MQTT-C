#include <stdio.h>
#include "MQTTClient.h"
#include "CayenneMQTTClient.h"
#include "MQTTLinux.h"
#include <stdio.h>

#include <stdbool.h>
#include <unistd.h>

bool checkMessages = false;
bool messageMatched = false;
bool messageReceived = false;
Network network;
CayenneMQTTClient mqttClient;
CayenneMessageData testMessage;


void usage(void)
{
	printf("Cayenne MQTT Test\n");
	printf("Usage: testmqtt <options>, where options are:\n");
	printf("  --host <hostname> (default is %s)\n", CAYENNE_DOMAIN);
	printf("  --port <port> (default is %d)\n", CAYENNE_PORT);
	printf("  --username <username> (default is username)\n");
	printf("  --password <password> (default is password)\n");
	printf("  --clientID <clientID> (default is clientID)\n");
	printf("  --help (show this)\n");
	exit(-1);
}


struct opts_struct
{
	char* username;
	char* password;
	char* clientID;
	char* host;
	int port;
} opts =
{
	(char*)"username", (char*)"password", (char*)"clientID", (char*)CAYENNE_DOMAIN, CAYENNE_PORT
};

char clientID2[] = "clientID2";


void getopts(int argc, char** argv)
{
	int count = 1;

	while (count < argc)
	{
		if (strcmp(argv[count], "--help") == 0)
		{
			usage();
		}
		else if (strcmp(argv[count], "--host") == 0)
		{
			if (++count < argc)
				opts.host = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--port") == 0)
		{
			if (++count < argc)
				opts.port = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--username") == 0)
		{
			if (++count < argc)
				opts.username = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--password") == 0)
		{
			if (++count < argc)
				opts.password = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--clientID") == 0)
		{
			if (++count < argc)
				opts.clientID = argv[count];
			else
				usage();
		}
		count++;
	}
	
}


void outputMessage(CayenneMessageData* message)
{
	int i;
	printf("topic=%d channel=%d", message->topic, message->channel);
	if (message->clientID) {
		printf(" clientID=%s", message->clientID);
	}
	if (message->type) {
		printf(" type=%s", message->type);
	}
	for (i = 0; i < message->valueCount; ++i) {
		if (message->values[i].unit) {
			printf(" unit=%s", message->values[i].unit);
		}
		if (message->values[i].value) {
			printf(" value=%s", message->values[i].value);
		}
	}
	if (message->id) {
		printf(" id=%s", message->id);
	}
}


void checkMessage(CayenneMessageData* message)
{
	int i;
	outputMessage(message);
	messageMatched = true;
	if (message->topic != testMessage.topic) {
		printf(" top err: %u\n", testMessage.topic);
		messageMatched = false;
	}
	if (message->channel != testMessage.channel) {
		printf(" chan err: %u\n", testMessage.channel);
		messageMatched = false;
	}
	if (((message->clientID != NULL) || (testMessage.clientID != NULL)) && (message->clientID && testMessage.clientID && strcmp(message->clientID, testMessage.clientID) != 0)) {
		printf(" devID err: %s\n", testMessage.clientID ? testMessage.clientID : "NULL");
		messageMatched = false;
	}
	if (((message->type != NULL) || (testMessage.type != NULL)) && (message->type && testMessage.type && strcmp(message->type, testMessage.type) != 0)) {
		printf(" type err: %s\n", testMessage.type ? testMessage.type : "NULL");
		messageMatched = false;
	}
	if (((message->id != NULL) || (testMessage.id != NULL)) && (message->id && testMessage.id && strcmp(message->id, testMessage.id) != 0)) {
		printf(" id err: %s\n", testMessage.id ? testMessage.id : "NULL");
		messageMatched = false;
	}
	if (message->valueCount != testMessage.valueCount) {
		printf(" valCount err: %u\n", testMessage.valueCount);
		messageMatched = false;
	}
	for (i = 0; i < message->valueCount; ++i) {
		if (((message->values[i].value != NULL) || (testMessage.values[i].value != NULL)) &&
			(message->values[i].value && testMessage.values[i].value && (strcmp(message->values[i].value, testMessage.values[i].value) != 0))) {
			printf(" val%d err: %s\n", i, testMessage.values[i].value ? testMessage.values[i].value : "NULL");
			messageMatched = false;
		}
		if (((message->values[i].unit != NULL) || (testMessage.values[i].unit != NULL)) &&
			(message->values[i].unit && testMessage.values[i].unit && (strcmp(message->values[i].unit, testMessage.values[i].unit) != 0))) {
			printf(" unit%d err: %s\n", i, testMessage.values[i].unit ? testMessage.values[i].unit : "NULL");
			messageMatched = false;
		}
	}
}


void messageArrived(CayenneMessageData* message)
{
	if (!checkMessages)
		return;
	printf(" -> messageArrived: ");
	checkMessage(message);
	if (message->topic == COMMAND_TOPIC && message->id)
	{
		int rc = CAYENNE_SUCCESS;
		if ((message->channel == 0) || (message->channel == 1)) {
			switch (message->channel) {
			case 0:
				rc = CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, "error message");
				break;
			case 1:
				rc = CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, NULL);
				break;
			}
			if(rc == CAYENNE_SUCCESS)
				printf(" Responded, rc: %d", rc);
			else
				printf(" Response failure, rc: %d", rc);
		}
	}
	messageReceived = true;
}


void configMessageArrived(CayenneMessageData* message)
{
	if (!checkMessages)
		return;
	printf(" -> configMessageArrived: ");
	checkMessage(message);

	int rc = CAYENNE_SUCCESS;
	if (message->channel == 0) {
		if ((rc = CayenneMQTTUnsubscribe(&mqttClient, message->clientID, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS)) == CAYENNE_SUCCESS)
			printf(" Unsubscribed, rc: %d", rc);
		else
			printf(" Unsubscribe failure, rc: %d", rc);
	}

	messageReceived = true;
}


void device2MessageArrived(CayenneMessageData* message)
{
	if (!checkMessages)
		return;
	printf(" -> device2MessageArrived: ");
	checkMessage(message);

	int rc = CAYENNE_SUCCESS;
	if (message->channel == 0) {
		if ((rc = CayenneMQTTUnsubscribe(&mqttClient, message->clientID, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS)) == CAYENNE_SUCCESS)
			printf(" Unsubscribed, rc: %d", rc);
		else
			printf(" Unsubscribe failure, rc: %d", rc);
	}

	messageReceived = true;
}


void subscribe(CayenneTopic topic, unsigned int channel, CayenneMessageHandler handler, const char* clientID)
{
	printf("Subscribe: topic=%d ", topic);
	int rc = CayenneMQTTSubscribe(&mqttClient, clientID ? clientID : mqttClient.clientID, topic, channel, handler);
	char buffer[CAYENNE_MAX_MESSAGE_SIZE] = { 0 };
	CayenneBuildTopic(buffer, sizeof(buffer), mqttClient.username, clientID ? clientID : mqttClient.clientID, topic, channel);
	printf(buffer);
	printf(", rc: %d\n", rc);
}


int connectClient(void)
{
	printf("Connecting to %s:%d\n", opts.host, opts.port);

	int rc = 0;
	while ((rc = NetworkConnect(&network, opts.host, opts.port)) != 0)
	{
		printf("TCP connect failed, rc: %d\n", rc);
		sleep(2);
	}

	printf("MQTT connecting\n");
	rc = CayenneMQTTConnect(&mqttClient);
	if (rc != 0) {
		printf("MQTT connect failed, rc: %d\n", rc);
		return rc;
	}
	else {
		printf("MQTT connected\n");
	}
	subscribe(COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL, clientID2);
	subscribe(CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, configMessageArrived, NULL);
	subscribe(CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, device2MessageArrived, clientID2);
	subscribe(DATA_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL);
	subscribe(SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL);
	subscribe(SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL);
	subscribe(SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL);
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	subscribe(DIGITAL_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(ANALOG_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(DIGITAL_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(DIGITAL_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(ANALOG_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
	subscribe(ANALOG_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL, NULL);
#endif
	return CAYENNE_SUCCESS;
}


bool waitForMessage(void)
{
	Timer timer;
	TimerInit(&timer);
	TimerCountdown(&timer, 1);
	messageReceived = false;
	while (!messageReceived && !TimerIsExpired(&timer))
	{
		CayenneMQTTYield(&mqttClient, 50);
		if (!NetworkConnected(&network) || !CayenneMQTTConnected(&mqttClient))
		{
			NetworkDisconnect(&network);
			CayenneMQTTDisconnect(&mqttClient);
			printf("Reconnecting\n");
			connectClient();
			break;
		}
	}
	return messageReceived;
}


void initTestMessage(CayenneTopic topic, unsigned int pin, char* value, char* type, char* unit, char* clientID)
{
	testMessage.id = NULL;
	testMessage.topic = topic;
	testMessage.channel = pin;
	testMessage.clientID = clientID ? clientID : mqttClient.clientID;
	testMessage.type = type;
	testMessage.values[0].value = value;
	testMessage.values[0].unit = unit;
	testMessage.valueCount = 1;
	printf("Publish: ");
	outputMessage(&testMessage);
}


void checkPublishSuccess(CayenneTopic topic, char* type, char* unit, bool shouldReceive)
{
	switch (topic)
	{
		//Some messages are parsed differently so update the test message to make the comparison work.
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	case ANALOG_TOPIC:
		testMessage.values[0].unit = unit;
		testMessage.values[0].value = type;
		testMessage.type = NULL;
		break;
	case DIGITAL_COMMAND_TOPIC:
	case ANALOG_COMMAND_TOPIC:
#endif
	case COMMAND_TOPIC:
		testMessage.id = type;
		testMessage.values[0].value = unit;
		testMessage.values[0].unit = NULL;
		testMessage.type = NULL;
		break;
	default:
		break;
	}
	waitForMessage();
	bool succeeded = false;
	if (messageReceived && shouldReceive && messageMatched) {
		succeeded = true;
	}
	else if (!messageReceived && !shouldReceive)
		succeeded = true;

	if (succeeded)
		printf(" - SUCCESS\n");
	else
		printf(" - FAILURE\n");
}


void testPublish(CayenneTopic topic, unsigned int pin, char* value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	initTestMessage(topic, pin, value, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishData(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishInt(CayenneTopic topic, unsigned int pin, int value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[2 + 8 * sizeof(value)];
	snprintf(str, sizeof(str), "%d", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataInt(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishUInt(CayenneTopic topic, unsigned int pin, unsigned int value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[1 + 8 * sizeof(value)];
	snprintf(str, sizeof(str), "%u", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataUInt(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishLong(CayenneTopic topic, unsigned int pin, long value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[2 + 8 * sizeof(value)];
	snprintf(str, sizeof(str), "%ld", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataLong(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishULong(CayenneTopic topic, unsigned int pin, unsigned long value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[2 + 8 * sizeof(value)];
	snprintf(str, sizeof(str), "%lu", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataULong(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishDouble(CayenneTopic topic, unsigned int pin, double value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[33];
	snprintf(str, 33, "%2.3f", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataDouble(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}


void testPublishFloat(CayenneTopic topic, unsigned int pin, float value, char* type, char* unit, bool shouldReceive, char* clientID)
{
	char str[33];
	snprintf(str, 33, "%2.3f", value);
	initTestMessage(topic, pin, str, type, unit, clientID);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataFloat(&mqttClient, clientID, topic, pin, type, unit, value)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(topic, type, unit, shouldReceive);
}

void initDataArrayTestMessage(CayenneDataArray* dataArray)
{
	unsigned int i;
	testMessage.id = NULL;
	testMessage.topic = DATA_TOPIC;
	testMessage.channel = 0;
	testMessage.clientID = mqttClient.clientID;
	testMessage.type = "type";
	for (i = 0; i < dataArray->valueCount; i++) {
		testMessage.values[i].value = dataArray->values[i].value;
		testMessage.values[i].unit = dataArray->values[i].unit;
	}
	testMessage.valueCount = dataArray->valueCount;
	printf("Publish: ");
	outputMessage(&testMessage);
}

void testDataArrayPublish(bool shouldReceive)
{
	CayenneDataArray dataArray;
	char buffer[100] = { 0 };
	CayenneDataArrayInit(&dataArray, buffer, sizeof(buffer));
	CayenneDataArrayAdd(&dataArray, "string", "val");
	CayenneDataArrayAddInt(&dataArray, "int", -1);
	CayenneDataArrayAddUInt(&dataArray, "uint", 2);
	CayenneDataArrayAddLong(&dataArray, "long", 3);
	
	initDataArrayTestMessage(&dataArray);
	int rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataArray(&mqttClient, NULL, testMessage.topic, testMessage.channel, testMessage.type, dataArray.values, dataArray.valueCount)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(testMessage.topic, NULL, NULL, shouldReceive);

	CayenneDataArrayClear(&dataArray);
	CayenneDataArrayAddULong(&dataArray, "ulong", 4);
	CayenneDataArrayAddDouble(&dataArray, "double", 123.4);
	CayenneDataArrayAddFloat(&dataArray, "float", -56.789);

	initDataArrayTestMessage(&dataArray);
	rc = CAYENNE_SUCCESS;
	if ((rc = CayenneMQTTPublishDataArray(&mqttClient, NULL, testMessage.topic, testMessage.channel, testMessage.type, dataArray.values, dataArray.valueCount)) != CAYENNE_SUCCESS) {
		printf(" Publish failed, rc: %d\n", rc);
		return;
	}
	checkPublishSuccess(testMessage.topic, NULL, NULL, shouldReceive);
}


int main(int argc, char** argv)
{
#ifdef PARSE_INFO_PAYLOADS
	bool parseInfoPayload = true;
#else
	bool parseInfoPayload = false;
#endif

	struct timeval before, after, elapsed;
	gettimeofday(&before, NULL);

	getopts(argc, argv);

	printf("Cayenne MQTT Test\n");
	NetworkInit(&network);
	CayenneMQTTClientInit(&mqttClient, &network, opts.username, opts.password, opts.clientID, messageArrived);
	if (connectClient() != CAYENNE_SUCCESS) {
		printf("Connection failed, exiting\n");
		if (CayenneMQTTConnected(&mqttClient))
			CayenneMQTTDisconnect(&mqttClient);
		if (NetworkConnected(&network))
			NetworkDisconnect(&network);
		return 1;
	}
	//Wait for any retained messages so they don't mess up the tests.
	waitForMessage();

	printf("Test Publish\n");
	checkMessages = true;
	testPublish(COMMAND_TOPIC, 0, NULL, "1", "respond with error", true, NULL);
	testPublish(COMMAND_TOPIC, 1, NULL, "2", "respond with ok", true, NULL);
	testPublish(COMMAND_TOPIC, 2, NULL, "1", "alternate clientID", true, clientID2);
	testPublish(CONFIG_TOPIC, 0, "off", NULL, NULL, true, NULL);
	testPublish(CONFIG_TOPIC, 1, "on", NULL, NULL, false, NULL);
	testPublish(CONFIG_TOPIC, 0, "off", NULL, NULL, true, clientID2);
	testPublish(CONFIG_TOPIC, 1, "on", NULL, NULL, false, clientID2);
	testPublish(DATA_TOPIC, 0, "10", TYPE_PROXIMITY, NULL, parseInfoPayload, NULL);
	testPublish(DATA_TOPIC, 0, "123", TYPE_TEMPERATURE, UNIT_CELSIUS, parseInfoPayload, NULL);
	testPublishInt(DATA_TOPIC, 0, -123, TYPE_TEMPERATURE, UNIT_KELVIN, parseInfoPayload, NULL);
	testPublishUInt(DATA_TOPIC, 0, 123, TYPE_TEMPERATURE, UNIT_FAHRENHEIT, parseInfoPayload, NULL);
	testPublishLong(DATA_TOPIC, 0, 123, TYPE_PROXIMITY, UNIT_CENTIMETER, parseInfoPayload, NULL);
	testPublishULong(DATA_TOPIC, 0, 123, TYPE_LUMINOSITY, UNIT_LUX, parseInfoPayload, NULL);
	testPublishDouble(DATA_TOPIC, 0, 123.4, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, parseInfoPayload, NULL);
	testPublishFloat(DATA_TOPIC, 0, 99.9, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT, parseInfoPayload, NULL);
	testDataArrayPublish(parseInfoPayload);
	testPublish(SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, "Model", NULL, NULL, parseInfoPayload, NULL);
	testPublish(SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, "1.0.0", NULL, NULL, parseInfoPayload, NULL);
	testPublish(SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, "CPU Model", NULL, NULL, parseInfoPayload, NULL);
	testPublish(SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, "1000000", NULL, NULL, parseInfoPayload, NULL);
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	testPublish(DIGITAL_TOPIC, 10, "1", NULL, NULL, parseInfoPayload, NULL);
	testPublish(ANALOG_TOPIC, 20, NULL, "124", "8", parseInfoPayload, NULL);
	testPublish(DIGITAL_COMMAND_TOPIC, 11, NULL, "1", "value", true, NULL);
	testPublish(DIGITAL_CONFIG_TOPIC, 12, "off", NULL, NULL, true, NULL);
	testPublish(ANALOG_COMMAND_TOPIC, 21, NULL, "2", "value", true, NULL);
	testPublish(ANALOG_CONFIG_TOPIC, 22, "on", NULL, NULL, true, NULL);
#endif

	if (CayenneMQTTConnected(&mqttClient))
		CayenneMQTTDisconnect(&mqttClient);
	testPublish(DATA_TOPIC, 0, NULL, "disconnected, should fail", NULL, true, NULL);
	if (NetworkConnected(&network))
		NetworkDisconnect(&network);

	gettimeofday(&after, NULL);
	timersub(&after, &before, &elapsed);
	printf("MQTT Test Finished, elapsed time %ld ms\n", (elapsed.tv_sec < 0) ? 0 : elapsed.tv_sec * 1000 + elapsed.tv_usec / 1000);

	return 0;
}


