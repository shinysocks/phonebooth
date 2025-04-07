## **phonebooth**
#### [technical writeup](technical_writeup.pdf)

### resources
>- [PortAudio/portaudio](https://github.com/PortAudio/portaudio)
>- [pa_blocking_read_write.html](https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html)
>- [installation_writeup](https://docs.google.com/document/d/1XouO8o9H_l_IDd2_Z0JVcL264ybqdvaisL009b8jgXM)
>- [raspberry_pi_3_model_b](https://www.adafruit.com/product/3055)
>- [usb_audio_adapter](https://www.adafruit.com/product/1475)
>- [usb_audio_card_setup_rpi](https://learn.adafruit.com/usb-audio-cards-with-a-raspberry-pi)

### photos
>[!Note]
>todo: Fine Arts Night - April 14th

### usage
>- ensure both [ALSA](https://www.alsa-project.org/wiki/Main_Page) and [make](https://www.gnu.org/software/make/manual/make.html) are installed
>
>- first compile the `libportaudio.a` library file for portaudio
  >    - [pa_compile_linux](https://portaudio.com/docs/v19-doxydocs/compile_linux.html)
>
>- build and run the project:
>```
>make && ./phonebooth
>```
