/*
    ██████╗ ██╗  ██╗ ██████╗ ███╗   ██╗███████╗    ██████╗  ██████╗  ██████╗ ████████╗██╗  ██╗    ███╗
    ██╔══██╗██║  ██║██╔═══██╗████╗  ██║██╔════╝    ██╔══██╗██╔═══██╗██╔═══██╗╚══██╔══╝██║  ██║    ╚██║
    ██████╔╝███████║██║   ██║██╔██╗ ██║█████╗      ██████╔╝██║   ██║██║   ██║   ██║   ███████║     ██║
    ██╔═══╝ ██╔══██║██║   ██║██║╚██╗██║██╔══╝      ██╔══██╗██║   ██║██║   ██║   ██║   ██╔══██║     ██║
    ██║     ██║  ██║╚██████╔╝██║ ╚████║███████╗    ██████╔╝╚██████╔╝╚██████╔╝   ██║   ██║  ██║    ███║
    ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝    ╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   ╚═╝  ╚═╝    ╚══╝

    source code for an interactive art installation by Daisy DiCarlo

    https://github.com/shinysocks/phonebooth
    https://github.com/PortAudio/portaudio
    https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html
    https://www.adafruit.com/product/3055
 */

#include <filesystem>
#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;
const string PHRASES_PATH = "phrases/";
const int FRAMES_PER_BUFFER = 128;
const int SAMPLE_RATE = 44100;
const int NUM_CHANNELS = 1;
PaStream* stream;

// SILENCE_CUTOFF: length of silence until a recording is ended
const double SILENCE_CUTOFF = (4.0 * SAMPLE_RATE) / FRAMES_PER_BUFFER;

// ACCEPTABLE_PHRASE_CUTOFF: an acceptable phrase is at least 4.0 seconds
const double ACCEPTABLE_PHRASE_CUTOFF = (8.0 * SAMPLE_RATE);

int end(PaError);
void play(vector<float> &phrase);
PaError initialize();

int main(int argc, char* argv[]) {
    PaError err = initialize();
    if (err != paNoError) end(err);
    vector<float> phrase;

    ifstream f(PHRASES_PATH + argv[1]);

    while (f) {
        float sample;
        f >> sample;
        phrase.push_back(sample);
    } f.close();

    play(phrase);

    err = Pa_StopStream(stream);
    if (err != paNoError) end(err);

    err = Pa_CloseStream(stream);
    if (err != paNoError) end(err);

    return end(err);
}

void play(vector<float> &phrase) {
    // skip the last 3 seconds
    int playback_duration = (phrase.size() / FRAMES_PER_BUFFER) - (3 * SAMPLE_RATE / FRAMES_PER_BUFFER);

    for (int i = 0; i < playback_duration; i++) {
        Pa_WriteStream(stream, &phrase[i * FRAMES_PER_BUFFER], FRAMES_PER_BUFFER);
    }

    phrase.clear();
}

PaError initialize() {
    PaStreamParameters* inputParams = new PaStreamParameters;
    PaStreamParameters* outputParams = new PaStreamParameters;
    PaError err;

    err = Pa_Initialize();

    // fill input and output parameters
    inputParams->device = Pa_GetDefaultInputDevice();
    inputParams->channelCount = NUM_CHANNELS;
    inputParams->sampleFormat = PA_SAMPLE_TYPE;
    inputParams->suggestedLatency = Pa_GetDeviceInfo(inputParams->device)->defaultHighInputLatency;
    inputParams->hostApiSpecificStreamInfo = NULL;

    outputParams->device = Pa_GetDefaultOutputDevice();
    outputParams->channelCount = NUM_CHANNELS;
    outputParams->sampleFormat = PA_SAMPLE_TYPE;
    outputParams->suggestedLatency = Pa_GetDeviceInfo(outputParams->device)->defaultHighOutputLatency;
    outputParams->hostApiSpecificStreamInfo = NULL;

    // opens input and output stream
    err = Pa_OpenStream(
            &stream,
            inputParams,
            outputParams,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,
            NULL,
            NULL);

    // start stream
    err = Pa_StartStream(stream);

    return err;
}

int end(PaError err) {
    err = Pa_Terminate();
    if (err != paNoError) {
        cerr << Pa_GetErrorText(err);
        return 1;
    }
    return 0;
}
