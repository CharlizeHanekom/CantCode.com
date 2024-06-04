main: main.cpp glad.c
	g++ -g shader.cpp shapesOpaque.cpp main.cpp camera.cpp glad.c -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o *.txt main

run:
	./main

all:
	make clean
	make
	make run