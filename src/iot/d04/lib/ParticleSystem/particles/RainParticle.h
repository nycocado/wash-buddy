#pragma once

#include "Particle.h"

/**
 * @struct RainSettings
 * @brief Configurações para o comportamento físico do efeito de Chuva.
 *
 * Define faixas de velocidade para diferentes intensidades de chuva e
 * dimensões visuais das gotas.
 */
struct RainSettings
{
        float lightMinSpeed = 60.0f;  ///< Velocidade mínima para chuva leve.
        float lightMaxSpeed = 100.0f; ///< Velocidade máxima para chuva leve.
        float heavyMinSpeed = 120.0f; ///< Velocidade mínima para chuva pesada.
        float heavyMaxSpeed = 180.0f; ///< Velocidade máxima para chuva pesada.
        float minLength = 3.0f;       ///< Comprimento visual mínimo da gota.
        float maxLength = 12.0f;      ///< Comprimento visual máximo da gota.
};

/**
 * @class RainParticle
 * @brief Representa uma partícula de chuva caindo verticalmente.
 *
 * Esta partícula simula gotas de chuva que caem em trajetórias lineares
 * verticais. Ao atingir o limite inferior do display, a gota entra em um estado
 * de "respingo" (splash), simulando a colisão com o solo.
 */
class RainParticle : public Particle
{
    public:
        /**
         * @brief Construtor da partícula de chuva.
         *
         * @param startX Posição horizontal inicial.
         * @param startY Posição vertical inicial.
         * @param cfg Referência para as configurações globais de chuva.
         * @param isHeavy Flag que define se a partícula deve usar velocidades
         * de chuva forte.
         */
        RainParticle(
            float startX,
            float startY,
            const RainSettings& cfg,
            bool isHeavy
        );

        /**
         * @brief Atualiza a física e o estado da gota de chuva.
         *
         * Realiza o movimento vertical e detecta a colisão com a base da tela.
         * Se colidir, inicia a contagem de tempo do efeito de respingo.
         *
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @param screenWidth Largura da tela em pixels.
         * @param screenHeight Altura da tela (ponto de impacto do respingo).
         * @return true se a gota ou o respingo ainda estão ativos no display.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /**
         * @brief Renderiza a gota ou o efeito de respingo.
         *
         * Desenha uma linha vertical para gotas em queda, ou um padrão de
         * dispersão de pixels para representar o impacto no solo.
         *
         * @param display Referência para o controle do display U8G2.
         */
        void draw(U8G2& display) override;

    private:
        float length;     ///< Comprimento da linha que representa a gota.
        bool isSplashing; ///< Indica se a gota está executando a animação de
                          ///< impacto.
        float splashTimer;        ///< Temporizador da duração do respingo.
        const RainSettings& _cfg; ///< Referência para configurações.
};
