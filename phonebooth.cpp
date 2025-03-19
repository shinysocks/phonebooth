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
const float AMBIENT_THRESHOLD = 0.15;
const int FRAMES_PER_BUFFER = 256;
const int SAMPLE_RATE = 44100;
const int NUM_CHANNELS = 1;
PaStream* stream;

// SILENCE_CUTOFF: length of silence until a recording is ended
const double SILENCE_CUTOFF = (4.0 * SAMPLE_RATE) / FRAMES_PER_BUFFER;

// ACCEPTABLE_PHRASE_CUTOFF: an acceptable phrase is at least 3.5 seconds
const double ACCEPTABLE_PHRASE_CUTOFF = (7.5 * SAMPLE_RATE);

int end(PaError);
double average_buffer_level(float[]);
void play(vector<float> &phrase);
void play_random_phrase();
PaError initialize();
void save_phrase_to_file(vector<float> &phrase);

int main() {
    PaError err = initialize();
    if (err != paNoError) end(err);

    float prev_buffer[FRAMES_PER_BUFFER];

    while (true) {
        float buffer[FRAMES_PER_BUFFER];
        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);

        if (average_buffer_level(buffer) > AMBIENT_THRESHOLD) {
            cout << "recording..." << endl;
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

            // is freshly recorded phrase long enough?
            if (phrase.size() > ACCEPTABLE_PHRASE_CUTOFF) {
                cout << "phrase is long enough to save." << endl;
                save_phrase_to_file(phrase);
            }
            play_random_phrase();

        } else {
            /* 
             * save `buffer` to `prev_buffer` in case the next is loud enough
             * to prevent phrase from starting too abruptly
             */ 
            copy(buffer, buffer + FRAMES_PER_BUFFER, prev_buffer);
        }
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) end(err);

    err = Pa_CloseStream(stream);
    if (err != paNoError) end(err);

    return end(err);
}

unsigned int count_saved_phrases() {
    unsigned int n = 0;
    for (auto _ : filesystem::directory_iterator(PHRASES_PATH))
        n++;
    return n;
}

void save_phrase_to_file(vector<float> &phrase) {
    auto n = count_saved_phrases();
    ofstream file(PHRASES_PATH + to_string(n));

    for (auto sample : phrase)
        file << sample << endl;

    file.close();
    cout << "saved: [" << PHRASES_PATH + to_string(n) << "]" << endl; 
}

void play(vector<float> &phrase) {
    // skip the last 4 seconds
    int playback_duration = (phrase.size() / FRAMES_PER_BUFFER) - (4 * SAMPLE_RATE / FRAMES_PER_BUFFER);

    for (int i = 0; i < playback_duration; i++) {
        Pa_WriteStream(stream, &phrase[i * FRAMES_PER_BUFFER], FRAMES_PER_BUFFER);
    }

    phrase.clear();
}

void play_random_phrase() {
    vector<float> phrase;
    int random_index = rand() % (count_saved_phrases());
    ifstream f(PHRASES_PATH + to_string(random_index));

    while (f) {
        float sample;
        f >> sample;
        phrase.push_back(sample);
    } f.close();

    play(phrase);
    cout << "played: [" << random_index << "]" << endl;
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
