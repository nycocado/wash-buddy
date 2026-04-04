#pragma once

#include "Particle.h"

/**
 * @struct BubbleSettings
 * @brief Configurações físicas e visuais para o efeito de Bolhas.
 */
struct BubbleSettings
{
        float minRadius = 2.0f; ///< Raio mínimo (pixels).
        float maxRadius = 5.0f; ///< Raio máximo (pixels).
        float minSpeed = 20.0f; ///< Velocidade mínima de subida (px/s).
        float maxSpeed = 50.0f; ///< Velocidade máxima de subida (px/s).
        float oscillationSpeed = 0.002f; ///< Velocidade da oscilação.
        float oscillationAmp = 0.3f;     ///< Amplitude da oscilação (pixels).
};

/**
 * @class BubbleParticle
 * @brief Partícula de bolha com subida senoidal.
 *
 * Simula bolhas de sabão que sobem oscilando horizontalmente e estouram.
 */
class BubbleParticle : public Particle
{
    public:
        /**
         * @brief Construtor da bolha.
         * @param startX Posição X inicial.
         * @param startY Posição Y inicial.
         * @param cfg Configurações de comportamento.
         */
        BubbleParticle(float startX, float startY, const BubbleSettings& cfg);

        /**
         * @brief Atualiza a física de subida e oscilação.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return True se a partícula continua ativa.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renderiza a bolha ou animação de estouro. */
        void draw(U8G2& display) override;

    private:
        float radius;
        float offset;
        bool isPopping;
        float popTimer;
        const BubbleSettings& _cfg;
};
