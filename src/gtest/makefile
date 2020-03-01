INC_DIR = -I ./src -I ./src/gtest -I ./src/server -I ./src/server/shared

csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cpp) \
		$(wildcard src/server/shared/networking/ip/*.cpp) \
		$(wildcard src/gtest/*.cpp)

dep = $(obj:.o=.d)

obj = $(csrc:.c=.o) $(ccsrc:.cpp=.o)

CXXFLAGS = -pthread -Wall -Wno-attributes $(INC_DIR)
debug = true

ifeq ($(debug), false)
	CXXFLAGS += -std=gnu++17 -O3 -DNDEBUG
else ifeq ($(debug), true)
	CXXFLAGS += -std=gnu++17 -g -O0
endif

LDFLAGS = -lstdc++fs -pthread /usr/local/lib/libgtest.a

PROGRAMS = test

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
