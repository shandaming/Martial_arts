INC_DIR = -I ./src	\
		  -I ./src/common	\
		  -I ./src/common/cmdline	\
		  -I ./src/common/configuration	\
		  -I ./src/common/cryptography	\
		  -I ./src/common/cryptography/authentication	\
		  -I ./src/common/debugging	\
		  -I ./src/common/logging	\
		  -I ./src/common/serialization	\
		  -I ./src/common/threading	\
		  -I ./src/common/utility	\
		  -I ./src/client	\
		  -I ./src/server	\
		  -I ./src/server/process	\
		  -I ./src/server/database	\
		  -I ./src/server/database/database	\
		  -I ./src/server/database/database/implementation	\
		  -I ./src/server/database/logging	\
		  -I ./src/server/database/update	\
		  -I ./src/server/game/world	\
		  -I ./src/server/game/server	\
		  -I ./src/server/game/server/protocol	\
		  -I ./src/server/game/server/packets	\
		  -I ./src/server/process	\
		  -I ./src/server/shared/networking	\
		  -I ./src/server/shared/networking/ip	\
		  -I ./src/server/shared/packets	\
		  -I ./src/server/worldserver

csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cpp) \
		$(wildcard src/common/cmdline/*.cpp) \
		$(wildcard src/common/configuration/*.cpp) \
		$(wildcard src/common/cryptograp/*.cpp) \
		$(wildcard src/common/cryptograp/authentication/*.cpp) \
		$(wildcard src/common/debugging/*.cpp) \
		$(wildcard src/common/logging/*.cpp) \
		$(wildcard src/common/serialization/*.cpp) \
		$(wildcard src/common/threading/*.cpp) \
		$(wildcard src/common/utility/*.cpp) \
		$(wildcard src/server/*.cpp) \
		$(wildcard src/server/database/database/*.cpp) \
		$(wildcard src/server/database/database/implementation/*.cpp) \
		$(wildcard src/server/database/update/*.cpp) \
		$(wildcard src/server/database/logging/*.cpp) \
		$(wildcard src/server/game/world/*.cpp) \
		$(wildcard src/server/game/server/*.cpp) \
		$(wildcard src/server/game/server/protocol/*.cpp) \
		$(wildcard src/server/game/server/packets/*.cpp) \
		$(wildcard src/server/process/*.cpp) \
		$(wildcard src/server/shared/networking/*.cpp) \
		$(wildcard src/server/shared/networking/ip/*.cpp) \
		$(wildcard src/server/shared/packets/*.cpp) \
		$(wildcard src/server/worldserver/*.cpp)

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
