csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cpp)

dep = $(obj:.o=.d)

obj = $(csrc:.c=.o) $(ccsrc:.cpp=.o)

CXXFLAGS = -std=c++1z -pthread -Wall -Wextra -Werror=non-virtual-dtor -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wold-style-cast -Wtrampolines -g -O0

LDFLAGS = -lstdc++fs

cmd: $(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) cmd

.PHONY: cleandep
cleandep:
	rm -f $(dep)
