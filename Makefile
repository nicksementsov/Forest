CFLAGS = -std=c++20 -O2 -Wall -DNBDEBUG=1
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

bin/Forest: main.cpp
	g++ $(CFLAGS) -o bin/Forest main.cpp $(LDFLAGS)

.PHONY: test clean

test: bin/Forest
	bin/Forest

clean:
	rm -f bin/Forest