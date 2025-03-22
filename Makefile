# Evan Hackstadt 2025 Winter CSCI 62

FLAGS=-std=c++20

output: network.o user.o post.o
	g++ $(FLAGS) $^ -o $@

test: main.o network.o user.o post.o
	g++ $(FLAGS) $^ -o $@

%.o: %.cpp
	g++ $(FLAGS) -c $^ -o $@

clean:
	rm *.o output test
