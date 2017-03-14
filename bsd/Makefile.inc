SRC_DIR=${TOPDIR}/src
PLATFORM_DIR=$(SRC_DIR)/Platform/Linux
EXAMPLES_DIR=$(PLATFORM_DIR)/examples
TESTS_DIR=$(PLATFORM_DIR)/tests

CFLAGS=	\
    -DLINUX_SO \
    -Wall \
    -Wstrict-prototypes \
    -O2 \
    -MMD \
    -MP \
    -I$(SRC_DIR)/CayenneMQTTClient \
    -I$(SRC_DIR)/MQTTCommon \
    -I$(MBEDTLS_DIR)/include \
    -I$(PLATFORM_DIR)

.OBJDIR: ${TOPDIR}/bsd/build

.PATH: $(SRC_DIR)/CayenneMQTTClient
.PATH: $(SRC_DIR)/CayenneUtils
.PATH: $(SRC_DIR)/MQTTCommon
.PATH: $(EXAMPLES_DIR)
.PATH: $(TESTS_DIR)
.PATH: $(PLATFORM_DIR)

MK_MAN=no

BASE_SRCS= \
	CayenneMQTTClient.c \
	CayenneUtils.c \
	MQTTPacket.c \
	MQTTUnsubscribeClient.c \
	MQTTClient.c \
	MQTTConnectClient.c \
	MQTTSerializePublish.c \
	Network.c \
	CayenneDataArray.c \
	MQTTDeserializePublish.c \
	MQTTSubscribeClient.c \
	Timer.c

