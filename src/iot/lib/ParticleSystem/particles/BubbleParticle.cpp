#include "BubbleParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Lifecycle */

BubbleParticle::BubbleParticle(
    float startX,
    float startY,
    const BubbleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Radius variation for visual diversity
    radius = _cfg.minRadius + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxRadius - _cfg.minRadius);

    // Rise speed (negative vertical in the screen's coordinate system)
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // Phase offset for varied sinusoidal trajectories
    offset = (float)random(0, 360);
    isPopping = false;
    popTimer = 0.0f;
}

/** @section Physics and Logic */

bool BubbleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isPopping)
    {
        // Uniform vertical movement
        y -= speed * deltaTime;

        // Sinusoidal horizontal oscillation
        x += sin(millis() * _cfg.oscillationSpeed + offset) *
             _cfg.oscillationAmp;

        // Pop logic: leaving the screen or statistical fragility
        if (y < -radius || (y < (screenHeight / 2) && random(0, 1000) < 5))
        {
            isPopping = true;
        }
        return true;
    }
    else
    {
        // Transient pop animation
        popTimer += deltaTime;
        return (popTimer < 0.1f);
    }
}

/** @section Rendering */

void BubbleParticle::draw(U8G2& display)
{
    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;
    int16_t ir = (int16_t)radius;

    if (!isPopping)
    {
        display.drawCircle(ix, iy, ir);

        // Reflective highlight based on the size
        if (ir == 2)
        {
            display.drawPixel(ix - 1, iy - 1);
        }
        else if (ir == 3)
        {
            display.drawPixel(ix - 1, iy - 2);
        }
        else if (ir == 4)
        {
            display.drawPixel(ix - 2, iy - 2);
            display.drawPixel(ix - 1, iy - 3);
        }
        else if (ir >= 5)
        {
            display.drawPixel(ix - 2, iy - 3);
            display.drawPixel(ix - 3, iy - 2);
        }
    }
    else
    {
        // Radial dispersion visual effect for the pop
        int spread = (int)(popTimer * 50.0f) + 1;
        display.drawPixel(ix - spread, iy - spread);
        display.drawPixel(ix + spread, iy - spread);
        display.drawPixel(ix - spread, iy + spread);
        display.drawPixel(ix + spread, iy + spread);
    }
}
