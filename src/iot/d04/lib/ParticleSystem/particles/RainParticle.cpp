#include "RainParticle.h"
#include <Arduino.h>

/**
 * @brief Configura a gota de chuva com parâmetros físicos baseados na
 * intensidade.
 */
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

    // A intensidade (heavy/light) determina a distribuição estatística da
    // velocidade. Chuva forte possui maior massa/energia cinética, resultando
    // em quedas mais rápidas.
    float minS = isHeavy ? _cfg.heavyMinSpeed : _cfg.lightMinSpeed;
    float maxS = isHeavy ? _cfg.heavyMaxSpeed : _cfg.lightMaxSpeed;

    // Velocidade linear descendente: v = v_min + rand(0,1) * (v_max - v_min)
    speed = minS + (float)(random(0, 100) / 100.0f) * (maxS - minS);

    // Comprimento visual da gota: simula o efeito de "motion blur" natural da
    // queda rápida.
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    isSplashing = false;
    splashTimer = 0.0f;
}

/**
 * @brief Atualiza a integração do movimento e a transição de estados físicos.
 */
bool RainParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isSplashing)
    {
        // Movimento Retilíneo Uniforme Vertical: dy = v * dt
        y += speed * deltaTime;

        // Detecção de colisão com o plano horizontal (chão)
        // O limite é definido pela altura da tela (screenHeight)
        if (y >= screenHeight)
        {
            isSplashing = true;
            y = (float)screenHeight; // Trava a coordenada Y no ponto de impacto
        }
        return true;
    }
    else
    {
        // Transição para estado de dissipação de energia (Respingo)
        splashTimer += deltaTime;

        // O efeito visual de respingo é efêmero (150ms de persistência)
        return (splashTimer < 0.15f);
    }
}

/**
 * @brief Renderização geométrica das gotas e respingos.
 */
void RainParticle::draw(U8G2& display)
{
    if (!isSplashing)
    {
        // Desenha o corpo da gota em queda: linha vertical de comprimento
        // 'length' A extremidade superior é (y - length) e a inferior é (y)
        display.drawVLine((int16_t)x, (int16_t)(y - length), (int16_t)length);
    }
    else
    {
        // Efeito de Respingo (Splash): Dispersão Radial Simplificada
        // Calcula o espalhamento horizontal lateral como função linear do tempo
        // decorrido
        int spread = (int)(splashTimer * 30.0f);
        int floorY = display.getHeight() - 1;

        // Projeção de gotas menores (pixels) saltando para os lados do ponto de
        // impacto
        display.drawPixel((int16_t)(x - spread - 1), floorY);
        display.drawPixel((int16_t)(x + spread + 1), floorY);

        // Simula o rebote vertical inicial da gota central: y_splash = floor -
        // t*v_splash
        if (splashTimer < 0.08f)
        {
            display.drawPixel((int16_t)x, floorY - spread);
        }
    }
}
