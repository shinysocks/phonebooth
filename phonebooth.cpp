/*
    PHONE BOOTH INSTALLATION
    Noah Dinan & Daisy DiCarlo

    https://github.com/PortAudio/portaudio
    https://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html
    https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html
    https://docs.google.com/document/d/1XouO8o9H_l_IDd2_Z0JVcL264ybqdvaisL009b8jgXM
 */

#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int NUM_CHANNELS = 1;
const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 256;
const float AMBIENT_THRESHOLD = 0.3;
const double SILENCE_CUTOFF = (5.0 * SAMPLE_RATE) / FRAMES_PER_BUFFER;
PaStream* stream;

int end(PaError);
double average_buffer_level(float[]);
void play(vector<float> &phrase);
PaError initialize();

int main() {
    PaError err = initialize();
    if (err != paNoError) end(err);

    float prev_buffer[FRAMES_PER_BUFFER];
    while (true) {
        float buffer[FRAMES_PER_BUFFER];
        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

        if (average_buffer_level(buffer) > AMBIENT_THRESHOLD) {
            cout << "started recording" << endl;
            int silence_counter = 0;
            vector<float> phrase;

            // copy previous buffer into phrase
            for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
                phrase.push_back(prev_buffer[j]);
            }

            while (silence_counter < SILENCE_CUTOFF) {
                double average = average_buffer_level(buffer);

                for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
                    phrase.push_back(buffer[j]);
                }

                Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

                if (average < AMBIENT_THRESHOLD) silence_counter++;
                else silence_counter = 0;
            }

            // is phrase vector length long enough?
            /*if (phrase.size() > 3 seconds) {}*/

            play(phrase);
        } else {
            // save buffer to prev_buffer in case the next is loud enough
            copy(buffer, buffer + FRAMES_PER_BUFFER, prev_buffer);
        }
    }

    // eventual read from file code
    /*vector<float> phrase(SAMPLE_RATE);*/
    /*while (file) {*/
    /*    float sample;*/
    /*    file >> sample;*/
    /*    phrase.push_back(sample);*/
    /*}*/
    /*file.close();*/

    err = Pa_StopStream(stream);
    if (err != paNoError) end(err);

    err = Pa_CloseStream(stream);
    if (err != paNoError) end(err);

    return end(err);
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

void play(vector<float> &phrase) {
    int playback_duration = (phrase.size() / FRAMES_PER_BUFFER) - (4 * SAMPLE_RATE / FRAMES_PER_BUFFER);

    for (int i = 0; i < playback_duration; i++) { // don't play 4 seconds at the end
        Pa_WriteStream(stream, &phrase[i * FRAMES_PER_BUFFER], FRAMES_PER_BUFFER);
    }

    cout << "played" << endl;
    phrase.clear();
}

double average_buffer_level(float buffer[]) {
    double average = 0.0;
    for (int j = 0; j < FRAMES_PER_BUFFER; j++)
        average += abs(buffer[j]);
    return (average / (double) FRAMES_PER_BUFFER);
}

int end(PaError err) {
    err = Pa_Terminate();
    if (err != paNoError) {
        cerr << Pa_GetErrorText(err);
        return 1;
    }
    return 0;
}
