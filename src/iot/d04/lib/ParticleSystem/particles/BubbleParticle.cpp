#include "BubbleParticle.h"
#include <Arduino.h>
#include <math.h>

/**
 * @brief Inicializa a bolha com posição, raio e velocidade aleatórios.
 */
BubbleParticle::BubbleParticle(
    float startX,
    float startY,
    const BubbleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // A variação de raio cria diversidade visual: r = min + [0,1] * (max - min)
    radius = _cfg.minRadius + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxRadius - _cfg.minRadius);

    // Velocidade de subida (vetor vertical negativo no sistema de coordenadas
    // do display)
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // O offset de fase garante que bolhas criadas simultaneamente não sigam a
    // mesma trajetória senoidal
    offset = (float)random(0, 360);
    isPopping = false;
    popTimer = 0.0f;
}

/**
 * @brief Executa a integração da física e detecção de colisões/estouro.
 */
bool BubbleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isPopping)
    {
        // Movimento retilíneo uniforme para cima: dy = v * dt
        y -= speed * deltaTime;

        // Oscilação Horizontal: x = x_base + sin(t * freq + phase) * amplitude
        // Simula o empuxo e a resistência lateral do fluido (ar/sabão)
        x += sin(millis() * _cfg.oscillationSpeed + offset) *
             _cfg.oscillationAmp;

        // Lógica de término de vida:
        // 1. Limite geométrico: quando o topo da bolha sai da área visível (y <
        // -radius)
        // 2. Probabilidade estatística: simula a fragilidade da bolha após
        // passar do meio da tela
        if (y < -radius || (y < (screenHeight / 2) && random(0, 1000) < 5))
        {
            isPopping = true;
        }
        return true;
    }
    else
    {
        // Animação de estouro: estado transiente antes da destruição do objeto
        popTimer += deltaTime;

        // Define a duração da persistência da "explosão" visual (100ms)
        return (popTimer < 0.1f);
    }
}

/**
 * @brief Renderização geométrica baseada no estado da partícula.
 */
void BubbleParticle::draw(U8G2& display)
{
    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;
    int16_t ir = (int16_t)radius;

    if (!isPopping)
    {
        // Desenha a circunferência da bolha
        display.drawCircle(ix, iy, ir);

        // Adiciona um ponto de reflexo (specular highlight) para sugerir volume
        // esférico O posicionamento do brilho é ajustado heuristicamente
        // conforme o raio
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
        // Efeito visual de dispersão (estouro):
        // Partículas menores (pixels) se afastam do centro radialmente a uma
        // velocidade constante
        int spread = (int)(popTimer * 50.0f) + 1;
        display.drawPixel(ix - spread, iy - spread);
        display.drawPixel(ix + spread, iy - spread);
        display.drawPixel(ix - spread, iy + spread);
        display.drawPixel(ix + spread, iy + spread);
    }
}
