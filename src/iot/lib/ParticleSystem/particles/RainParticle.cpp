#include "RainParticle.h"
#include <Arduino.h>

/** @section Lifecycle */

RainParticle::RainParticle(
    float startX,
    float startY,
    const RainSettings& cfg,
    bool isHeavy
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Speed based on the rain's intensity
    float minS = isHeavy ? _cfg.heavyMinSpeed : _cfg.lightMinSpeed;
    float maxS = isHeavy ? _cfg.heavyMaxSpeed : _cfg.lightMaxSpeed;
    speed = minS + (float)(random(0, 100) / 100.0f) * (maxS - minS);

    // Drop length (simulated motion blur)
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    isSplashing = false;
    splashTimer = 0.0f;
}

/** @section Physics and Logic */

bool RainParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isSplashing)
    {
        y += speed * deltaTime;

        // Collision detection with the ground (bottom of the screen)
        if (y >= screenHeight)
        {
            isSplashing = true;
            y = (float)screenHeight;
        }
        return true;
    }
    else
    {
        // Dissipation state (splash)
        splashTimer += deltaTime;
        return (splashTimer < 0.15f);
    }
}

/** @section Rendering */

void RainParticle::draw(U8G2& display)
{
    if (!isSplashing)
    {
        display.drawVLine((int16_t)x, (int16_t)(y - length), (int16_t)length);
    }
    else
    {
        // Simplified radial splash effect
        int spread = (int)(splashTimer * 30.0f);
        int floorY = display.getHeight() - 1;

        display.drawPixel((int16_t)(x - spread - 1), floorY);
        display.drawPixel((int16_t)(x + spread + 1), floorY);

        // Ephemeral center bounce
        if (splashTimer < 0.08f)
        {
            display.drawPixel((int16_t)x, floorY - spread);
        }
    }
}
