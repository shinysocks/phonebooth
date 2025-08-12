.SILENT:
compile:
	g++ -Wall *.cpp libportaudio.a -lrt -lm -lasound -pthread -g -o phonebooth

build:
	rm -rf _pa
	git clone https://github.com/PortAudio/portaudio _pa
	cd _pa && ./configure && sudo make install
	echo "move .a file"
	cd .. && rm -rf _pa
	cp /usr/local/lib/libportaudio.a .
