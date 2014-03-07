all: kmmp

kmmp: *.cpp
	g++ -o $@ $^

clean:
	@rm kmmp
