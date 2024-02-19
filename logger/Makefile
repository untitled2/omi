CC=g++
CXXFLAGS=-std=c++20 #-Wall -Wextra #-pedantic
OBJS=logger_test.o
PROGRAM=logger_test

all: $(PROGRAM)

$(PROGRAM):	logger_test.o

test_logger.o:	logger_test.cpp

clean:
	rm -f $(OBJS) $(PROGRAM)

