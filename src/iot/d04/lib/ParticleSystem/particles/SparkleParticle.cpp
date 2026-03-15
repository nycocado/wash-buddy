#include "SparkleParticle.h"
#include <Arduino.h>
#include <math.h>

/**
 * @brief Inicializa a cintilação com estado dinâmico aleatório.
 */
SparkleParticle::SparkleParticle(
    float startX,
    float startY,
    const SparkleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Velocidade de subida (vetor vertical ascendente): v_up = v_min +
    // rand(0,1) * Δv
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // Define uma fase inicial φ em [0, π] para que as partículas brilhem em
    // tempos diferentes
    blinkTimer = (float)random(0, 314) / 100.0f;
}

/**
 * @brief Atualiza a integração da trajetória e o temporizador de efeito.
 */
bool SparkleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    // Movimento Retilíneo Uniforme Vertical Ascendente: dy = -v * dt
    y -= speed * deltaTime;

    // Evolução Temporal da Cintilação: dφ = ω * dt
    // Onde ω (velocidade angular) é definida por blinkSpeed
    blinkTimer += _cfg.blinkSpeed * deltaTime;

    // Condição de saída: quando a partícula flutua para fora da margem superior
    // do display
    return (y > -5);
}

/**
 * @brief Renderiza a estrela através de discretização de uma função de brilho
 * contínua.
 */
void SparkleParticle::draw(U8G2& display)
{
    // Cálculo de Intensidade (I): I = |sin(φ)| * escala
    // Como sin(φ) varia de -1 a 1, normalizamos para 0 a 3 pixels de raio
    // aparente
    float intensity = (sin(blinkTimer) + 1.0f) * 1.5f;

    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;

    // Mapeamento Discreto de Intensidade para Geometria (Rasterização de cruz):
    // Estágio 1: Pixel central (núcleo da estrela)
    if (intensity > 0.5f)
    {
        display.drawPixel(ix, iy);
    }
    // Estágio 2: Cruz de 1 pixel (cintilação média)
    if (intensity > 1.5f)
    {
        display.drawPixel(ix + 1, iy);
        display.drawPixel(ix - 1, iy);
        display.drawPixel(ix, iy + 1);
        display.drawPixel(ix, iy - 1);
    }
    // Estágio 3: Cruz de 2 pixels (brilho máximo)
    if (intensity > 2.5f)
    {
        display.drawPixel(ix + 2, iy);
        display.drawPixel(ix - 2, iy);
        display.drawPixel(ix, iy + 2);
        display.drawPixel(ix, iy - 2);
    }
}
