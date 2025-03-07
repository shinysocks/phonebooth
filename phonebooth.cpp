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
#include <vector>

using namespace std;

const int NUM_CHANNELS = 1;
const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 128;
const float AMBIENT_THRESHOLD = 0.2;
const int TIME_RUNNING = 5;

int main() {
    PaStreamParameters* inputParams = new PaStreamParameters;
    PaStreamParameters* outputParams = new PaStreamParameters;
    PaStream* stream;
    PaError err;
    vector<vector<float>> phrases = vector<vector<float>>();

    err = Pa_Initialize();
    if (err != paNoError) goto end;

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
    if (err != paNoError) goto end;

    // start stream
    err = Pa_StartStream(stream);
    if (err != paNoError) goto end;


    // PSEUDOCODE FROM NOTEBOOK (GENIUS CODE)
    //
    // Phrase[] phrases
    // int silence_counter = 0;
    // Phrase phrase;
    //
    // while (silence_counter < (180 * SAMPLE_RATE)) {
    //      record buffer -> phrase
    //      if (level < 0.5) {
    //          silence_counter++;
    //      } else {
    //          silence_counter = 0;
    //      }
    // }
    //
    // if (phrase longer than 4 seconds) {
    //      save phrase to phrases
    // }
    //
    // playback the phrase or random phrase from phrases

    float five_second_clip[(TIME_RUNNING * SAMPLE_RATE) / FRAMES_PER_BUFFER][FRAMES_PER_BUFFER];

    // loop passing data from input to output
    for (int i = 0; i < (TIME_RUNNING * SAMPLE_RATE) / FRAMES_PER_BUFFER; i++) {
        float buffer[FRAMES_PER_BUFFER];
        vector<float> vector_buffer (FRAMES_PER_BUFFER);

        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

        vector_buffer.assign(FRAMES_PER_BUFFER, *buffer);

        phrases.push_back(vector_buffer);

        double average = 0.0;

        for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
            five_second_clip[i][j] = buffer[j];
            average += abs(buffer[j]);
        }


        // average value for a buffer
        if ((average / (double) FRAMES_PER_BUFFER) > AMBIENT_THRESHOLD) {
            cout << "|||||||||||||||" << endl;
        } else {
            cout << "." << endl;
        }
    }

    cout << "Playing back to you!" << endl;
    Pa_Sleep(500);

    // playback from array
    for (int i = 0; i < (TIME_RUNNING * SAMPLE_RATE) / FRAMES_PER_BUFFER; i++) {
        /*Pa_WriteStream(stream, &phrases[i], FRAMES_PER_BUFFER);*/
        /*cout << phrases[i] << " : " << five_second_clip[i] << endl;*/
        Pa_WriteStream(stream, five_second_clip[i], FRAMES_PER_BUFFER);
    }

    /*for (float phrase : *phrases) {*/
    /*    cout << phrase << endl;*/
    /*    Pa_WriteStream(stream, &phrase, FRAMES_PER_BUFFER); // how to pass a reference to the phrase?*/
    /*}*/

    end:

    err = Pa_StopStream(stream);
    err = Pa_CloseStream(stream);

    err = Pa_Terminate();
    if (err != paNoError) {
        cerr << Pa_GetErrorText(err);
        return 1;
    }
    return 0;
}
