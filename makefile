.SILENT:
all:
	g++ -Wall *.cpp libportaudio.a portaudio.h -lrt -lm -lasound -pthread -g -o main
	./main
