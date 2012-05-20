#include "timer.hh"

Timer::Timer()
{
    this->last_ = SDL_GetTicks();
}

void Timer::adjust()
{
    unsigned int delta = SDL_GetTicks() - this->last_;

    printf("Displayed frame in %d milliseconds.\n", delta);
    if (delta < ELAPSED_FRAME)
    {
        SDL_Delay(ELAPSED_FRAME / 1000 - delta);
    }
    this->last_ = SDL_GetTicks();
}
