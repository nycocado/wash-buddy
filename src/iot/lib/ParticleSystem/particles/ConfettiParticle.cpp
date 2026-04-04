#include "ConfettiParticle.h"
#include <Arduino.h>
#include <math.h>

/** @section Ciclo de Vida */

ConfettiParticle::ConfettiParticle(
    float startX,
    float startY,
    const ConfettiSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Trajetória inicial em ângulo aleatório
    float angle = (float)random(0, 360) * (M_PI / 180.0f);
    float mag = _cfg.minSpeed + (float)(random(0, 100) / 100.0f) *
                                    (_cfg.maxSpeed - _cfg.minSpeed);

    vx = cos(angle) * mag;
    vy = sin(angle) * mag;

    // Diversidade visual: tamanho e giro
    size = (float)random(2, 5);
    spinSpeed = (float)random(5, 20);
    spinPhase = (float)random(0, 314) / 100.0f;
}

/** @section Física e Lógica */

bool ConfettiParticle::update(
    float deltaTime,
    int screenWidth,
    int screenHeight
)
{
    // Integração simples de Euler
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Gravidade e Resistência do ar
    vy += _cfg.gravity * deltaTime;
    vx *= _cfg.airResistance;
    vy *= _cfg.airResistance;

    // Rotação visual (flicker)
    spinPhase += spinSpeed * deltaTime;

    // Condição de morte: sair pelas bordas
    return (y < screenHeight && x >= -5 && x <= screenWidth + 5);
}

/** @section Renderização */

void ConfettiParticle::draw(U8G2& display)
{
    // Projeção 3D simplificada (largura oscilante)
    float currentWidth = abs(cos(spinPhase)) * size;

    int drawW = max(1, (int)currentWidth);
    int drawH = (int)size;

    display.drawBox(
        (int16_t)(x - drawW / 2.0f), (int16_t)(y - drawH / 2.0f), drawW, drawH
    );
}
