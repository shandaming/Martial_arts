INC_DIR = -I ./src -I ./src/client -I ./src/server

csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cpp) \
		$(wildcard src/common/cmdline/*.cpp) \
		$(wildcard src/common/configuration/*.cpp) \
		$(wildcard src/common/debugging/*.cpp) \
		$(wildcard src/common/logging/*.cpp) \
		$(wildcard src/common/serialization/*.cpp) \
		$(wildcard src/common/utility/*.cpp) \
		$(wildcard src/server/*.cpp) \
		$(wildcard src/server/database/*.cpp) \
		$(wildcard src/server/database/update/*.cpp) \
		$(wildcard src/server/database/logging/*.cpp) \
		$(wildcard src/server/net/*.cpp) \
		$(wildcard src/server/process/*.cpp)

dep = $(obj:.o=.d)

obj = $(csrc:.c=.o) $(ccsrc:.cpp=.o)

CXXFLAGS = -pthread -Wall -Wno-attributes $(INC_DIR)
debug = true

ifeq ($(debug), false)
	CXXFLAGS += -std=gnu++17 -O3 -DNDEBUG
else ifeq ($(debug), true)
	CXXFLAGS += -std=gnu++17 -g -O0
endif

LDFLAGS = -lstdc++fs

PROGRAMS = service

$(PROGRAMS): $(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(PROGRAMS)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

DESTDIR = ""
PREFIX = "$prefix"

.PHONY: install
install: $(PROGRAMS)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(PROGRAMS)

.PHONY: unistall
unistall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROGRAMS)
