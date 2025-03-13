/*
    PHONE BOOTH INSTALLATION
    Noah Dinan & Daisy DiCarlo

    https://github.com/PortAudio/portaudio
    https://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html
    https://portaudio.com/docs/v19-doxydocs/blocking_read_write.html
    https://docs.google.com/document/d/1XouO8o9H_l_IDd2_Z0JVcL264ybqdvaisL009b8jgXM
 */


// refactor todo:
//      move PA stuff to class
//      move average to func
//      fix error handling
//      more file operations to func
//      clean up main while loop


#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int NUM_CHANNELS = 1;
const PaSampleFormat PA_SAMPLE_TYPE = paFloat32;
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 128;
const float AMBIENT_THRESHOLD = 0.3;
PaStream* stream;

int end(PaError);

int main() {
    PaStreamParameters* inputParams = new PaStreamParameters;
    PaStreamParameters* outputParams = new PaStreamParameters;
    PaError err;

    int silence_counter = 0;
    int length = 0;

    err = Pa_Initialize();
    if (err != paNoError) end(err);

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
    if (err != paNoError) end(err);

    // start stream
    err = Pa_StartStream(stream);
    if (err != paNoError) end(err);

    Pa_Sleep(1000);

    float prev_buffer[FRAMES_PER_BUFFER];

    while (true) {
        double average = 0.0;
        float buffer[FRAMES_PER_BUFFER];

        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

        // get_average_level_for_buffer()
        for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
            average += abs(buffer[j]);
        }
        average = (average / (double) FRAMES_PER_BUFFER);

        if (average > AMBIENT_THRESHOLD) {
            cout << "starting recording!" << endl;
            // record buffer and start recording and waiting for silence

            int silence_counter = 0;
            vector<float> phrase;

            // copy previous buffer into phrase
            for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
                phrase.push_back(prev_buffer[j]);
            }

            while (silence_counter < (2 * SAMPLE_RATE) / FRAMES_PER_BUFFER) { // MAKE THIS PRETTIER
                double average = 0.0;

                for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
                    phrase.push_back(buffer[j]);
                    average += abs(buffer[j]);
                }

                average = (average / (double) FRAMES_PER_BUFFER);

                Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

                if (average < AMBIENT_THRESHOLD) silence_counter++;
                else silence_counter = 0;
            }
            // is phrase vector length long enough?
            // if (phrase.size() > 3 seconds) {}

            // play!!
            for (unsigned int i = 0; i < phrase.size() / FRAMES_PER_BUFFER; i++) {
                Pa_WriteStream(stream, &phrase[i * FRAMES_PER_BUFFER], FRAMES_PER_BUFFER);
            }
        } else {
            // save this buffer in case the next is loud enough?
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

int end(PaError err) {
    err = Pa_Terminate();
    if (err != paNoError) {
        cerr << Pa_GetErrorText(err);
        return 1;
    }
    return 0;
}
