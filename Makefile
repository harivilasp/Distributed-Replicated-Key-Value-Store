#-------------------------------------------------------------------------------
# Setting up necessary variables for the build process
#-------------------------------------------------------------------------------

CC := gcc
CXX := g++
CFLAGS := -Wall -std=c++11
LFLAGS := -pthread

# Automatic detection of source and header files
HDRS := $(wildcard *.h)
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

SVR_SRCS := $(wildcard Server*.cpp)
SVR_OBJS := $(SVR_SRCS:.cpp=.o)

# Exclude ServerMain.cpp from SVR_SRCS for the load target
SVR_SRCS_NO_MAIN := $(filter-out ServerMain.cpp, $(SVR_SRCS))
SVR_OBJS_NO_MAIN := $(SVR_SRCS_NO_MAIN:.cpp=.o)

CLNT_SRCS := $(wildcard Client*.cpp)
CLNT_OBJS := $(CLNT_SRCS:.cpp=.o)

LOAD_SRCS := $(wildcard Load*.cpp)
LOAD_OBJS := $(LOAD_SRCS:.cpp=.o)

# For load executable, include Load* and Server* source files excluding ServerMain.cpp
LOAD_ALL_SRCS := $(LOAD_SRCS) $(SVR_SRCS_NO_MAIN)
LOAD_ALL_OBJS := $(LOAD_ALL_SRCS:.cpp=.o)

CMN_SRCS := $(filter-out $(SVR_SRCS) $(CLNT_SRCS) $(LOAD_SRCS), $(SRCS))
CMN_OBJS := $(CMN_SRCS:.cpp=.o)

TARGET := server client load

#-------------------------------------------------------------------------------
# Build rules
#-------------------------------------------------------------------------------

all: $(TARGET)

debug: CFLAGS += -ggdb -DDEBUG
debug: $(TARGET)

# Server executable
server: $(SVR_OBJS) $(CMN_OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

# Client executable
client: $(CLNT_OBJS) $(CMN_OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

# Load executable, now including Server* objects but excluding ServerMain.o
load: $(LOAD_OBJS) $(SVR_OBJS_NO_MAIN) $(CMN_OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

# Generic rule for compiling object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -rf *.o $(TARGET)

.PHONY: all debug clean
