#Makefile for building Linux C example and test executables

SRC_DIR := src
PLATFORM_DIR := $(SRC_DIR)/Platform/Linux
EXAMPLES_DIR := $(PLATFORM_DIR)/examples
TESTS_DIR := $(PLATFORM_DIR)/tests
BUILD_DIR := build
CC := gcc
CFLAGS := -Wall -Wstrict-prototypes -O2 -MMD -MP -I$(SRC_DIR)/CayenneMQTTClient -I$(SRC_DIR)/MQTTCommon -I$(MBEDTLS_DIR)/include -I$(PLATFORM_DIR)

#Paths containing source files
vpath %c $(SRC_DIR)/CayenneMQTTClient:$(SRC_DIR)/CayenneUtils:$(SRC_DIR)/MQTTCommon:$(EXAMPLES_DIR):$(TESTS_DIR):$(PLATFORM_DIR)

COMMON_SOURCES := $(notdir $(wildcard $(SRC_DIR)/CayenneMQTTClient/*.c) $(wildcard $(SRC_DIR)/CayenneUtils/*.c) $(wildcard $(SRC_DIR)/MQTTCommon/*.c) $(PLATFORM_DIR)/Timer.c)
COMMON_OBJS := $(COMMON_SOURCES:.c=.o)
NETWORK_OBJS := Network.o

#Ojbects and dependency files for examples
SIMPLE_PUBLISH_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) $(NETWORK_OBJS) SimplePublish.o)
SIMPLE_SUBSCRIBE_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) $(NETWORK_OBJS) SimpleSubscribe.o)
CLIENT_EXAMPLE_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) $(NETWORK_OBJS) CayenneClient.o)

#Ojbects and dependency files for tests
TEST_CLIENT_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) $(NETWORK_OBJS) TestClient.o)

.PHONY: all examples tests clean mostlyclean

all: examples tests

examples: simplepub simplesub cayenneclient

tests: testclient

simplepub: $(SIMPLE_PUBLISH_OBJS)
	$(CC) $(CFLAGS) $(SIMPLE_PUBLISH_OBJS) -o $@
    
simplesub: $(SIMPLE_SUBSCRIBE_OBJS)
	$(CC) $(CFLAGS) $(SIMPLE_SUBSCRIBE_OBJS) -o $@
	
cayenneclient: $(CLIENT_EXAMPLE_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_EXAMPLE_OBJS) -o $@

testclient: $(TEST_CLIENT_OBJS)
	$(CC) $(CFLAGS) $(TEST_CLIENT_OBJS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<	

clean:
	rm -r -f $(BUILD_DIR)
	rm -f simplepub simplesub cayenneclient testclient

-include $(BUILD_DIR)/*.d 
