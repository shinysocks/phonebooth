.SILENT:
all:
	g++ -Wall *.cpp libportaudio.a -lrt -lm -lasound -pthread -g -o phonebooth
