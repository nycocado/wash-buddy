#pragma once

#include "Particle.h"

/**
 * @struct WindSettings
 * @brief Configurações para o comportamento físico do efeito de Vento.
 *
 * Define os parâmetros que controlam a velocidade horizontal e as
 * dimensões das rajadas de vento representadas por linhas.
 */
struct WindSettings
{
        float minSpeed = 150.0f; ///< Velocidade horizontal mínima (pixels/s).
        float maxSpeed = 250.0f; ///< Velocidade horizontal máxima (pixels/s).
        float minLength = 5.0f;  ///< Comprimento mínimo da rajada (pixels).
        float maxLength = 20.0f; ///< Comprimento máximo da rajada (pixels).
};

/**
 * @class WindParticle
 * @brief Representa uma linha de vento que se move horizontalmente com
 * ondulação.
 *
 * Esta partícula simula rajadas de vento representadas por linhas horizontais
 * que possuem uma leve oscilação vertical baseada em uma função de onda
 * senoidal, dando uma sensação de fluidez e turbulência ao ar.
 */
class WindParticle : public Particle
{
    public:
        /**
         * @brief Construtor da partícula de vento.
         *
         * Inicializa a rajada com velocidade, comprimento e parâmetros de
         * ondulação vertical (frequência e amplitude) aleatórios.
         *
         * @param startX Posição horizontal inicial.
         * @param startY Posição vertical de base da rajada.
         * @param cfg Referência para as configurações globais de vento.
         */
        WindParticle(float startX, float startY, const WindSettings& cfg);

        /**
         * @brief Atualiza a posição horizontal da rajada de vento.
         *
         * Move a rajada para a direita linearmente.
         *
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @param screenWidth Largura da tela em pixels.
         * @param screenHeight Altura da tela em pixels.
         * @return true se a cauda da linha de vento ainda estiver visível na
         * área da tela.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /**
         * @brief Renderiza a linha ondulada de vento ponto a ponto.
         *
         * Calcula a altura Y de cada segmento da rajada aplicando uma senoide
         * sobre a coordenada X, criando um efeito visual de fluxo de ar
         * ondulado.
         *
         * @param display Referência para o controle do display U8G2.
         */
        void draw(U8G2& display) override;

    private:
        float baseY;  ///< Eixo vertical central (altura de repouso) da rajada.
        float length; ///< Comprimento horizontal total da rajada.
        float waveFrequency; ///< Frequência espacial da oscilação (ondulação
                             ///< por pixel).
        float waveAmplitude; ///< Amplitude máxima do deslocamento vertical
                             ///< (pixels).
        float phase; ///< Fase inicial da oscilação para variação visual.
        const WindSettings& _cfg; ///< Referência para configurações.
};
