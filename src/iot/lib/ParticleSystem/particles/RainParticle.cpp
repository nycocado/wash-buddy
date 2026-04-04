#include "RainParticle.h"
#include <Arduino.h>

/** @section Ciclo de Vida */

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

    // Velocidade baseada na intensidade da chuva
    float minS = isHeavy ? _cfg.heavyMinSpeed : _cfg.lightMinSpeed;
    float maxS = isHeavy ? _cfg.heavyMaxSpeed : _cfg.lightMaxSpeed;
    speed = minS + (float)(random(0, 100) / 100.0f) * (maxS - minS);

    // Comprimento da gota (motion blur simulado)
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    isSplashing = false;
    splashTimer = 0.0f;
}

/** @section Física e Lógica */

bool RainParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isSplashing)
    {
        y += speed * deltaTime;

        // Detecção de colisão com o solo (base da tela)
        if (y >= screenHeight)
        {
            isSplashing = true;
            y = (float)screenHeight;
        }
        return true;
    }
    else
    {
        // Estado de dissipação (respingo)
        splashTimer += deltaTime;
        return (splashTimer < 0.15f);
    }
}

/** @section Renderização */

void RainParticle::draw(U8G2& display)
{
    if (!isSplashing)
    {
        display.drawVLine((int16_t)x, (int16_t)(y - length), (int16_t)length);
    }
    else
    {
        // Efeito de Splash radial simplificado
        int spread = (int)(splashTimer * 30.0f);
        int floorY = display.getHeight() - 1;

        display.drawPixel((int16_t)(x - spread - 1), floorY);
        display.drawPixel((int16_t)(x + spread + 1), floorY);

        // Rebote central efêmero
        if (splashTimer < 0.08f)
        {
            display.drawPixel((int16_t)x, floorY - spread);
        }
    }
}
