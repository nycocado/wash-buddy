#include "BubbleParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Ciclo de Vida */

BubbleParticle::BubbleParticle(
    float startX,
    float startY,
    const BubbleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Variação de raio para diversidade visual
    radius = _cfg.minRadius + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxRadius - _cfg.minRadius);

    // Velocidade de subida (vertical negativa no sistema da tela)
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // Offset de fase para trajetórias senoidais variadas
    offset = (float)random(0, 360);
    isPopping = false;
    popTimer = 0.0f;
}

/** @section Física e Lógica */

bool BubbleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isPopping)
    {
        // Movimento vertical uniforme
        y -= speed * deltaTime;

        // Oscilação horizontal senoidal
        x += sin(millis() * _cfg.oscillationSpeed + offset) *
             _cfg.oscillationAmp;

        // Lógica de estouro: saída da tela ou fragilidade estatística
        if (y < -radius || (y < (screenHeight / 2) && random(0, 1000) < 5))
        {
            isPopping = true;
        }
        return true;
    }
    else
    {
        // Animação transiente de estouro
        popTimer += deltaTime;
        return (popTimer < 0.1f);
    }
}

/** @section Renderização */

void BubbleParticle::draw(U8G2& display)
{
    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;
    int16_t ir = (int16_t)radius;

    if (!isPopping)
    {
        display.drawCircle(ix, iy, ir);

        // Brilho reflexivo conforme o tamanho
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
        // Efeito visual de dispersão radial do estouro
        int spread = (int)(popTimer * 50.0f) + 1;
        display.drawPixel(ix - spread, iy - spread);
        display.drawPixel(ix + spread, iy - spread);
        display.drawPixel(ix - spread, iy + spread);
        display.drawPixel(ix + spread, iy + spread);
    }
}
