SRC_DIR=${TOPDIR}/src
LINUX_DIR=$(SRC_DIR)/Platform/Linux
EXAMPLES_DIR=$(LINUX_DIR)/examples
TESTS_DIR=$(LINUX_DIR)/tests

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
    -I$(LINUX_DIR)

.OBJDIR: $(SRC_DIR)/Platform/FreeBSD/build

.PATH: $(SRC_DIR)/CayenneMQTTClient
.PATH: $(SRC_DIR)/CayenneUtils
.PATH: $(SRC_DIR)/MQTTCommon
.PATH: $(EXAMPLES_DIR)
.PATH: $(TESTS_DIR)
.PATH: $(LINUX_DIR)

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

