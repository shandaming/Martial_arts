SRC_DIRS := .

# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)

# String substitution for every C/C++ file. As an example, hello.cpp turns into hello.cpp.o
OBJS := $(SRCS:.cpp=.o)

# String substitution (suffix version without %). As an example, hello.cpp.o turns into hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us! These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

vpath %.c %.cpp %.h $(INC_DIRS)

GCC_STANDARD_FLAGS := -std=gnu++17 -pthread -Wall -Wno-attributes $(INC_FLAGS)
GCC_RELEASE_FLAGS := -g -O3 -DNDEBUG
GCC_DEBUG_FLAGS := -g -O0

CFLAGS := $(GCC_STANDARD_FLAGS)
CXXFLAGS := $(GCC_STANDARD_FLAGS)

debug := true

ifeq ($(debug), false)
	CFLAGS += $(GCC_RELEASE_FLAGS)
	CXXFLAGS += $(GCC_RELEASE_FLAGS)
else ifeq ($(debug), true)
	CFLAGS += $(GCC_DEBUG_FLAGS)
	CXXFLAGS += $(GCC_RELEASE_FLAGS)
endif

LDFLAGS := -lstdc++fs -lgtest
PROGRAMS := service

$(PROGRAMS): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGRAMS) $(DEPS)

# Include the .d makefiles
-include $(DEPS)
