csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cpp) \
		$(wildcard src/common/cmdline/*.cpp) \
		$(wildcard src/server/*.cpp) \
		$(wildcard src/server/database/*.cpp) \
		$(wildcard src/server/database/update/*.cpp)

dep = $(obj:.o=.d)

obj = $(csrc:.c=.o) $(ccsrc:.cpp=.o)

CXXFLAGS = -std=c++1z -g -O0 -pthread -Wall -Wno-attributes
debug = true

ifeq ($(debug), false)
CXXFLAGS = -std=c++1z -O3 -pthread -Wall -Wno-attributes
else ifeq ($(debug), true)
CXXFLAGS = -std=c++1z -g -O0 -pthread -Wall -Wno-attributes
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
