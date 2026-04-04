#pragma once

#include "Particle.h"

/**
 * @struct RainSettings
 * @brief Configurações para o efeito de Chuva.
 */
struct RainSettings
{
        float lightMinSpeed = 60.0f;  ///< Velocidade mínima (chuva leve).
        float lightMaxSpeed = 100.0f; ///< Velocidade máxima (chuva leve).
        float heavyMinSpeed = 120.0f; ///< Velocidade mínima (chuva pesada).
        float heavyMaxSpeed = 180.0f; ///< Velocidade máxima (chuva pesada).
        float minLength = 3.0f;       ///< Comprimento mínimo da gota.
        float maxLength = 12.0f;      ///< Comprimento máximo da gota.
};

/**
 * @class RainParticle
 * @brief Partícula de chuva com efeito de respingo (splash).
 *
 * Simula gotas caindo verticalmente que se transformam em respingos ao colidir.
 */
class RainParticle : public Particle
{
    public:
        /**
         * @brief Construtor da gota.
         * @param startX Posição horizontal.
         * @param startY Posição vertical.
         * @param cfg Configurações de chuva.
         * @param isHeavy Define se usa parâmetros de chuva forte.
         */
        RainParticle(
            float startX,
            float startY,
            const RainSettings& cfg,
            bool isHeavy
        );

        /**
         * @brief Atualiza queda e temporizador de respingo.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Ponto de colisão.
         * @return True se gota ou respingo ainda estão ativos.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renderiza a gota em queda ou efeito de impacto. */
        void draw(U8G2& display) override;

    private:
        float length;
        bool isSplashing;
        float splashTimer;
        const RainSettings& _cfg;
};
