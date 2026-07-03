#include "ConfettiParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Lifecycle */

ConfettiParticle::ConfettiParticle(
    float startX,
    float startY,
    const ConfettiSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Random-angle initial trajectory
    float angle = (float)random(0, 360) * (M_PI / 180.0f);
    float mag = _cfg.minSpeed + (float)(random(0, 100) / 100.0f) *
                                    (_cfg.maxSpeed - _cfg.minSpeed);

    vx = cos(angle) * mag;
    vy = sin(angle) * mag;

    // Visual diversity: size and spin
    size = (float)random(2, 5);
    spinSpeed = (float)random(5, 20);
    spinPhase = (float)random(0, 314) / 100.0f;
}

/** @section Physics and Logic */

bool ConfettiParticle::update(
    float deltaTime,
    int screenWidth,
    int screenHeight
)
{
    // Simple Euler integration
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Gravity and air resistance
    vy += _cfg.gravity * deltaTime;
    vx *= _cfg.airResistance;
    vy *= _cfg.airResistance;

    // Visual rotation (flicker)
    spinPhase += spinSpeed * deltaTime;

    // Death condition: leaving through the edges
    return (y < screenHeight && x >= -5 && x <= screenWidth + 5);
}

/** @section Rendering */

void ConfettiParticle::draw(U8G2& display)
{
    // Simplified 3D projection (oscillating width)
    float currentWidth = abs(cos(spinPhase)) * size;

    int drawW = max(1, (int)currentWidth);
    int drawH = (int)size;

    display.drawBox(
        (int16_t)(x - drawW / 2.0f), (int16_t)(y - drawH / 2.0f), drawW, drawH
    );
}
