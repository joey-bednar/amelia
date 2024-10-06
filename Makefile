############################## DEFAULTS ##############################

DEFAULT_CFLAGS := -MMD -MP -Wall -Wextra -O3
DEFAULT_LDFLAGS := -lm

LINUX_CC := gcc
LINUX_CFLAGS :=
LINUX_LDFLAGS :=

WINDOWS_CC := x86_64-w64-mingw32-gcc-posix
WINDOWS_CFLAGS := --static -pthread
WINDOWS_LDFLAGS := --static -pthread -Wl,--stack,8388608

OUT := amelia

BUILD_DIR := ./build
SRC_DIRS := ./src

############################## CONFIGURATION ##############################

# set default target OS using kernel info
KERNEL := $(shell uname -s)
ifeq ($(TARGET),)
	ifeq ($(KERNEL),Windows_NT)
		TARGET := Windows
	else
		TARGET := Linux
	endif
endif


# set default compiler for target
ifneq ($(CC),)
	ifeq ($(TARGET),Windows)
		CC := $(WINDOWS_CC)
		OS_FLAGS := $(DEFAULT_CFLAGS) $(WINDOWS_CFLAGS)
		OS_LDFLAGS := $(DEFAULT_LDFLAGS) $(WINDOWS_LDFLAGS)
	else
		CC := $(LINUX_CC)
		OS_FLAGS := $(DEFAULT_CFLAGS) $(LINUX_CFLAGS)
		OS_LDFLAGS := $(DEFAULT_LDFLAGS) $(LINUX_LDFLAGS)
	endif
endif


# set march arguments
ifneq ($(ARCH),)
	MARCH := -march=$(value ARCH)
endif

############################## COMPILATION ##############################

# find all files to format
FORMAT_SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.h')

# find all files to compile
SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(INC_FLAGS) $(OS_FLAGS) $(MARCH)

LDFLAGS := $(OS_LDFLAGS)

# link
$(BUILD_DIR)/$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# compile
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

-include $(DEPS)

############################## EXTRAS ##############################

.PHONY: format
format:
	clang-format -i --style=file $(FORMAT_SRCS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

