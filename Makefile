all: kmmp

kmmp: src/*.cpp
	g++ -o $@ $^

clean:
	@rm kmmp
