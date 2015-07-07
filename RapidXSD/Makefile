
INCLUDES = -I/home/zkf/nebula/3rd_party/rapidxml

all: a.out
check: all
	./a.out
clean:
	-rm a.out

a.out: rapidxsd.h rapidxsd.tcc test-rapidxsd.cpp
	g++ -g $(INCLUDES) test-rapidxsd.cpp

