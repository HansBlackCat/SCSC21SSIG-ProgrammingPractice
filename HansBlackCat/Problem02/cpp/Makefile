CC = clang++
CPPFLAGS = -pthread -Wall -Wpedantic

release:
	$(CC) $(CPPFLAGS) main.cpp -o run

.PHONY: clean
clean:
	rm ./run
