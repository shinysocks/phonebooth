## **Phone Booth**
Source code for an interactive art installation by Daisy DiCarlo entitled "Phone Booth".
#### [Confession & Phone Booth Interaction Prompts](interaction_prompts.pdf)
#### [Technical Writeup](technical_writeup.pdf)

### resources
>- [PortAudio/portaudio](https://github.com/PortAudio/portaudio)
>- [pa_blocking_read_write.html](https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html)
>- [installation_writeup](https://docs.google.com/document/d/1XouO8o9H_l_IDd2_Z0JVcL264ybqdvaisL009b8jgXM)
>- [raspberry_pi_3_model_b](https://www.adafruit.com/product/3055)
>- [usb_audio_adapter](https://www.adafruit.com/product/1475)
>- [usb_audio_card_setup_rpi](https://learn.adafruit.com/usb-audio-cards-with-a-raspberry-pi)

### photos
>![phone_booth_full](https://github.com/user-attachments/assets/b7aaa8a6-6af6-4ba0-bad0-9cf907591be2)
>![phone_booth_close](https://github.com/user-attachments/assets/0ffa0b70-8ee9-473a-aeab-1a045e6d444b)

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
