#pragma once

#include "Particle.h"

/**
 * @struct ConfettiSettings
 * @brief Configurações físicas para o efeito de Confete.
 */
struct ConfettiSettings
{
        float minSpeed = 40.0f;      ///< Velocidade inicial mínima (px/s).
        float maxSpeed = 100.0f;     ///< Velocidade inicial máxima (px/s).
        float gravity = 30.0f;       ///< Gravidade aplicada (px/s^2).
        float airResistance = 0.98f; ///< Fator de resistência do ar.
};

/**
 * @class ConfettiParticle
 * @brief Partícula de confete com física de projétil e rotação.
 *
 * Simula a queda irregular de papéis coloridos com rotação 3D simplificada.
 */
class ConfettiParticle : public Particle
{
    public:
        /**
         * @brief Construtor do confete.
         * @param startX Posição X de lançamento.
         * @param startY Posição Y de lançamento.
         * @param cfg Configurações de física.
         */
        ConfettiParticle(
            float startX,
            float startY,
            const ConfettiSettings& cfg
        );

        /**
         * @brief Atualiza trajetória e rotação visual.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return True se a partícula ainda está visível.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renderiza o confete com efeito de rotação. */
        void draw(U8G2& display) override;

    private:
        float vx, vy;
        float size;
        float spinSpeed;
        float spinPhase;
        const ConfettiSettings& _cfg;
};
