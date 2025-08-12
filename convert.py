import scipy.io.wavfile as wf
import numpy as np
from sys import argv
from os import system

RATE = 44100

file = open(argv[1])
data = file.readlines()
file.close()

np_arr = np.array(data, dtype=np.float32)

wf.write(f'{argv[1]}.wav', RATE, np_arr)

system(f"ffmpeg -i {argv[1]}.wav {argv[1]}.mp3")
