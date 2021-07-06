CXX = g++

OBJS1 = prime_gen.o prime_main.o
TARGET1 = prime_main
OBJS2 = prime_gen.o prime_factorial_main.o
TARGET2 = prime_factorial_main
OBJS3 = prime_gen.o prime_leyland_main.o
TARGET3 = prime_leyland_main
OBJS4 = prime_gen.o prime_goldbach_main.o
TARGET4 = prime_goldbach_main

SRCS = $(OBJS1:.o=.cpp) $(OBJS2:.o=.cpp) $(OBJS3:.o=.cpp) $(OBJS4:.o=.cpp)

all : $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET1) : $(OBJS1)
	$(CXX) -o $@ $^

$(TARGET2) : $(OBJS2)
	$(CXX) -o $@ $^

$(TARGET3) : $(OBJS3)
	$(CXX) -o $@ $^

$(TARGET4) : $(OBJS4)
	$(CXX) -o $@ $^

clean:
	rm *.o *.exe
