SPECS    += arm_v7 armhf
CC_MARCH ?= -march=armv7-a -mfpu=vfp -mfloat-abi=hard

include $(BASE_DIR)/mk/spec/arm_v7.mk

