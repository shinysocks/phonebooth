/*
    Phone Booth Installation
    Noah Dinan & Daisy DiCarlo

    https://github.com/PortAudio/portaudio
    https://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html
    https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html
    https://docs.google.com/document/d/1XouO8o9H_l_IDd2_Z0JVcL264ybqdvaisL009b8jgXM
 */

#include "portaudio.h"
#include "iostream"

using namespace std;

const int NUM_CHANNELS = 1;
const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 128;

int main() {
    PaStreamParameters* inputParams;
    PaStreamParameters* outputParams;
    PaStream *stream;
    PaError err;

    cout << Pa_GetVersionText();

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    // fill input and output parameters
    inputParams->device = Pa_GetDefaultInputDevice();
    inputParams->channelCount = NUM_CHANNELS;
    inputParams->sampleFormat = PA_SAMPLE_TYPE;
    inputParams->suggestedLatency = Pa_GetDeviceInfo(inputParams->device)->defaultHighInputLatency ;
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
    cout << "stream started";


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


    // loop passing data from input to output
    for (int i = 0; i < (60*SAMPLE_RATE) / FRAMES_PER_BUFFER; i++) {
        void* buffer;
        err = Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
        err = Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
    }

    error:
        cout << "error";
        

    // cleanup
    err = Pa_StopStream( stream );
    err = Pa_CloseStream( stream );

    Pa_Terminate();
    return 0;
}


