#ifndef BEEPER_H
#define BEEPER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <queue>
#include <cmath>


struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
private:
    double v;

    const int AMPLITUDE = 12000;
    const int FREQUENCY = 44100;

    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

void audio_callback(void*, Uint8*, int);

#endif // BEEPER_H
