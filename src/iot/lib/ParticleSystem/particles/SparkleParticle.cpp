#include "SparkleParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Ciclo de Vida */

SparkleParticle::SparkleParticle(
    float startX,
    float startY,
    const SparkleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Velocidade de subida aleatória
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // Fase inicial para brilhos dessincronizados
    blinkTimer = (float)random(0, 314) / 100.0f;
}

/** @section Física e Lógica */

bool SparkleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    y -= speed * deltaTime;
    blinkTimer += _cfg.blinkSpeed * deltaTime;

    return (y > -5);
}

/** @section Renderização */

void SparkleParticle::draw(U8G2& display)
{
    // Intensidade baseada em senoide (0 a 3px de raio)
    float intensity = (sin(blinkTimer) + 1.0f) * 1.5f;

    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;

    // Desenha cruz de brilho conforme intensidade
    if (intensity > 0.5f)
    {
        display.drawPixel(ix, iy);
    }
    if (intensity > 1.5f)
    {
        display.drawPixel(ix + 1, iy);
        display.drawPixel(ix - 1, iy);
        display.drawPixel(ix, iy + 1);
        display.drawPixel(ix, iy - 1);
    }
    if (intensity > 2.5f)
    {
        display.drawPixel(ix + 2, iy);
        display.drawPixel(ix - 2, iy);
        display.drawPixel(ix, iy + 2);
        display.drawPixel(ix, iy - 2);
    }
}
