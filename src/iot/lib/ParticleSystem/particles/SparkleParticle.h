#pragma once

#include "Particle.h"

/**
 * @struct SparkleSettings
 * @brief Configurações para o efeito de Brilho/Cintilação.
 */
struct SparkleSettings
{
        float minSpeed = 5.0f;    ///< Velocidade mínima de subida.
        float maxSpeed = 15.0f;   ///< Velocidade máxima de subida.
        float blinkSpeed = 10.0f; ///< Frequência da cintilação (rad/s).
};

/**
 * @class SparkleParticle
 * @brief Partícula de brilho que pisca enquanto flutua.
 *
 * Simula estrelas ou brilhos de limpeza através de oscilação senoidal.
 */
class SparkleParticle : public Particle
{
    public:
        /**
         * @brief Construtor do brilho.
         * @param startX Posição horizontal.
         * @param startY Posição vertical.
         * @param cfg Configurações de brilho.
         */
        SparkleParticle(float startX, float startY, const SparkleSettings& cfg);

        /**
         * @brief Atualiza brilho e movimento ascendente.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return True se a partícula ainda está na tela.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renderiza a estrela com tamanho variável (cruz). */
        void draw(U8G2& display) override;

    private:
        float blinkTimer;
        const SparkleSettings& _cfg;
};
