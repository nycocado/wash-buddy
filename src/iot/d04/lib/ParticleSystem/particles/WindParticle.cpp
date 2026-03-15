#include "WindParticle.h"
#include <Arduino.h>
#include <math.h>

/**
 * @brief Inicializa a rajada de vento com cinemática e propriedades
 * ondulatórias aleatórias.
 */
WindParticle::WindParticle(float startX, float startY, const WindSettings& cfg)
    : _cfg(cfg)
{
    x = startX;
    baseY = startY;
    y = startY;

    // Velocidade Horizontal Constante (Vento Uniforme): dx/dt = v
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);

    // Dimensão horizontal da rajada
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    // Parâmetros da Função de Onda: y(x) = baseY + A * sin(k*x + φ)
    // waveFrequency (k): controla a densidade de picos e vales por unidade de
    // comprimento.
    waveFrequency = (float)random(5, 15) / 100.0f;
    // waveAmplitude (A): deslocamento vertical máximo em relação ao eixo de
    // equilíbrio (baseY).
    waveAmplitude = (float)random(2, 6);
    // phase (φ): garante que diferentes rajadas iniciem em pontos diferentes do
    // ciclo senoidal.
    phase = (float)random(0, 314) / 100.0f;
}

/**
 * @brief Realiza a translação horizontal da rajada.
 */
bool WindParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    // Movimento Retilíneo Uniforme Horizontal: dx = v * dt
    x += speed * deltaTime;

    // Validação de Limites Visuais: a partícula expira quando sua extremidade
    // esquerda (x - length) ultrapassa a borda direita da tela (screenWidth).
    return (x - length < (float)screenWidth);
}

/**
 * @brief Renderiza a rajada como uma série discreta de pontos ondulados.
 */
void WindParticle::draw(U8G2& display)
{
    int startX = (int)x;
    int endX = startX - (int)length;

    // Amostragem da função de onda ao longo do eixo X:
    // Para cada pixel que compõe o comprimento da rajada:
    for (int currX = startX; currX > endX; currX--)
    {
        // Clipe horizontal preventivo para evitar cálculos fora da área do
        // buffer do display
        if (currX >= 0 && currX < display.getWidth())
        {
            // Aplicação da Função de Onda Estacionária no referencial da
            // partícula: y = baseY + Amplitude * sin(x * Frequência + Fase)
            // Isso cria o efeito visual de "tremulação" ou "ondulação" do
            // vento.
            int currY = (int
            )(baseY + sin((float)currX * waveFrequency + phase) * waveAmplitude
            );
            display.drawPixel(currX, currY);
        }
    }
}
