# CalCPVFit Makefile

CXX      = g++
CXXFLAGS = -std=c++14 -Wall -O2 -g
ROOT_CFLAGS = $(shell root-config --cflags)
ROOT_LIBS   = $(shell root-config --libs)
MINUIT_LIBS = -lMinuit

TARGET  = CalCPVFit

SRCS    = main.cpp \
          src/ScaleConfigReader.cpp \
          src/HistLoader.cpp \
          src/CPVFitter.cpp

OBJS    = $(SRCS:.cpp=.o)

all: $(TARGET)
	@echo "Build complete: $(TARGET)"

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(ROOT_CFLAGS) -o $@ $^ $(ROOT_LIBS) $(MINUIT_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(ROOT_CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
	@echo "Clean complete"

.PHONY: all clean
