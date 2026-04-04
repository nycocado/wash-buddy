#pragma once

#include "Particle.h"

/**
 * @struct WindSettings
 * @brief Configurações físicas para o efeito de Vento.
 */
struct WindSettings
{
        float minSpeed = 150.0f; ///< Velocidade horizontal mínima (px/s).
        float maxSpeed = 250.0f; ///< Velocidade horizontal máxima (px/s).
        float minLength = 5.0f;  ///< Comprimento mínimo da rajada.
        float maxLength = 20.0f; ///< Comprimento máximo da rajada.
};

/**
 * @class WindParticle
 * @brief Partícula de vento com movimento ondulado.
 *
 * Simula rajadas de ar através de linhas que oscilam verticalmente.
 */
class WindParticle : public Particle
{
    public:
        /**
         * @brief Construtor da rajada.
         * @param startX Posição horizontal inicial.
         * @param startY Posição vertical de base.
         * @param cfg Configurações de vento.
         */
        WindParticle(float startX, float startY, const WindSettings& cfg);

        /**
         * @brief Atualiza a translação horizontal.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return True se a rajada ainda está visível.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renderiza a linha ondulada ponto a ponto. */
        void draw(U8G2& display) override;

    private:
        float baseY;
        float length;
        float waveFrequency;
        float waveAmplitude;
        float phase;
        const WindSettings& _cfg;
};
