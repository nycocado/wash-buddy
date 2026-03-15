#include "ConfettiParticle.h"
#include <Arduino.h>
#include <math.h>

/**
 * @brief Inicializa o confete com direção, velocidade e rotação aleatórias.
 */
ConfettiParticle::ConfettiParticle(
    float startX,
    float startY,
    const ConfettiSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;

    // Cálculo da trajetória inicial: ângulo aleatório (0 a 2π radianos)
    float angle = (float)random(0, 360) * (M_PI / 180.0f);
    // Magnitude da força de lançamento inicial
    float mag = _cfg.minSpeed + (float)(random(0, 100) / 100.0f) *
                                    (_cfg.maxSpeed - _cfg.minSpeed);

    // Decomposição polar para cartesiana: vx = cos(θ)*v, vy = sin(θ)*v
    vx = cos(angle) * mag;
    vy = sin(angle) * mag;

    // Parâmetros de diversidade visual: tamanho aleatório e velocidade angular
    size = (float)random(2, 5);
    spinSpeed = (float)random(5, 20);
    spinPhase = (float)random(0, 314) / 100.0f;
}

/**
 * @brief Resolve a integração da física para o quadro atual.
 */
bool ConfettiParticle::update(
    float deltaTime,
    int screenWidth,
    int screenHeight
)
{
    // Integração de Euler (simplificada): ds = v * dt
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Aceleração Vertical (Gravidade): dv_y = g * dt
    // Puxa a partícula constantemente para baixo
    vy += _cfg.gravity * deltaTime;

    // Resistência Aerodinâmica: v = v * k
    // Reduz a energia cinética da partícula em ambos os eixos ao longo do tempo
    vx *= _cfg.airResistance;
    vy *= _cfg.airResistance;

    // Atualiza a fase de rotação: dφ = ω * dt
    // Controla o efeito de "flicker" ou "giro" do papel retangular
    spinPhase += spinSpeed * deltaTime;

    // Condição de morte: sair pela base (y > h) ou lateralmente fora de uma
    // margem segura
    return (y < screenHeight && x >= -5 && x <= screenWidth + 5);
}

/**
 * @brief Desenha a partícula projetando sua rotação 3D em 2D.
 */
void ConfettiParticle::draw(U8G2& display)
{
    // Projeção Visual de Rotação: largura = base_size * |cos(φ)|
    // Isso simula o confete "vibrando" ou girando enquanto cai no espaço 3D
    float currentWidth = abs(cos(spinPhase)) * size;

    int drawW = max(1, (int)currentWidth);
    int drawH = (int)size;

    // Renderiza o retângulo centrado na posição calculada da partícula
    display.drawBox(
        (int16_t)(x - drawW / 2.0f), (int16_t)(y - drawH / 2.0f), drawW, drawH
    );
}
