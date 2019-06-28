ifndef TOP
 TOP = .
 INCLUDED = no
endif

include $(TOP)/config.mak

ifeq (-$(CC)-$(GCC_MAJOR)-$(findstring $(GCC_MINOR),56789)-,-gcc-4--)
 CXXFLAGS += -D_FORTIFY_SOURCE=0
endif

#SOURCE := $(wildcard *.c) $(wildcard *.cpp) 
SOURCE = src/main.cpp
OBJS = $(patsubst %.cpp,%.o,$(SOURCE)))

SEARCHPATH += src/ src/common src/db src/net src/common/log
vpath %.cpp $(SEARCHPATH)

CXXFLAGS += -I$(TOP)
CXXFLAGS += $(CPPFLAGS)
VPATH = $(TOPSRC)

EXESUF=
CC = g++
FILES=main.cpp

PROG = Gserver
OBJ=$(SRC:.cpp=.o)

all: $(PROG)

# compiling other source files.
%.o: %.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) -o $<

# linking the program.
$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)

config.mak:
	$(if $(wildcard $@),,@echo "Please run ./configure." && exit 1

clean:
	rm -f $(PROG)
	rm -f  *~ *.o *.a *.so* *.out *.log lib*.def *.exe *.dll a.out tags TAGS

distclean: clean
	rm -f config.mak

help:
	@echo "make"
	@echo "   build native compiler (from separate objects)"
	@echo ""
