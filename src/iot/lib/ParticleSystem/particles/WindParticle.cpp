#include "WindParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Lifecycle */

WindParticle::WindParticle(float startX, float startY, const WindSettings& cfg)
    : _cfg(cfg)
{
    x = startX;
    baseY = startY;
    y = startY;

    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    // Wave function parameters for vertical undulation
    waveFrequency = (float)random(5, 15) / 100.0f;
    waveAmplitude = (float)random(2, 6);
    phase = (float)random(0, 314) / 100.0f;
}

/** @section Physics and Logic */

bool WindParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    x += speed * deltaTime;
    return (x - length < (float)screenWidth);
}

/** @section Rendering */

void WindParticle::draw(U8G2& display)
{
    int startX = (int)x;
    int endX = startX - (int)length;

    // Draws the gust pixel by pixel, applying the sine function
    for (int currX = startX; currX > endX; currX--)
    {
        if (currX >= 0 && currX < display.getWidth())
        {
            int currY =
                (int)(baseY + sin((float)currX * waveFrequency + phase) *
                                  waveAmplitude);
            display.drawPixel(currX, currY);
        }
    }
}
