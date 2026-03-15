#pragma once

#include "Particle.h"

/**
 * @struct BubbleSettings
 * @brief Configurações para o comportamento físico e visual do efeito de
 * Bolhas.
 *
 * Contém os parâmetros que definem limites de tamanho, velocidade e oscilação
 * das partículas de bolha geradas pelo sistema.
 */
struct BubbleSettings
{
        float minRadius = 2.0f; ///< Raio mínimo da bolha em pixels.
        float maxRadius = 5.0f; ///< Raio máximo da bolha em pixels.
        float minSpeed =
            20.0f; ///< Velocidade mínima de subida (pixels/segundo).
        float maxSpeed =
            50.0f; ///< Velocidade máxima de subida (pixels/segundo).
        float oscillationSpeed =
            0.002f; ///< Velocidade da oscilação horizontal.
        float oscillationAmp =
            0.3f; ///< Amplitude da oscilação horizontal (pixels).
};

/**
 * @class BubbleParticle
 * @brief Representa uma partícula de bolha que sobe com oscilação horizontal.
 *
 * Esta partícula simula o comportamento de bolhas de sabão, subindo na tela
 * e estourando após atingir certa altura ou aleatoriamente. Utiliza uma
 * função senoidal para simular o balanço natural de bolhas de ar na água.
 */
class BubbleParticle : public Particle
{
    public:
        /**
         * @brief Construtor da partícula de bolha.
         *
         * Inicializa a posição, rádio e velocidades aleatórias baseadas nas
         * configurações.
         *
         * @param startX Posição X inicial no display.
         * @param startY Posição Y inicial no display.
         * @param cfg Referência para a estrutura de configurações de bolha.
         */
        BubbleParticle(float startX, float startY, const BubbleSettings& cfg);

        /**
         * @brief Atualiza a lógica e a física da bolha.
         *
         * Calcula o deslocamento vertical linear e o deslocamento horizontal
         * senoidal. Verifica as condições de estouro (topo da tela ou chance
         * aleatória).
         *
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @param screenWidth Largura total da tela em pixels.
         * @param screenHeight Altura total da tela em pixels.
         * @return true se a partícula ainda deve ser processada, false se
         * terminou sua vida (estourou).
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /**
         * @brief Renderiza a bolha ou sua animação de estouro no display.
         *
         * Desenha o círculo da bolha com um pequeno ponto de brilho, ou
         * uma animação de dispersão de 4 pontos se estiver no estado de
         * estouro.
         *
         * @param display Referência para o objeto de controle do display U8G2.
         */
        void draw(U8G2& display) override;

    private:
        float radius;   ///< Raio atual da bolha.
        float offset;   ///< Deslocamento de fase para a oscilação horizontal.
        bool isPopping; ///< Flag que indica se a bolha iniciou o processo de
                        ///< estouro.
        float popTimer; ///< Acumulador de tempo para a duração da animação de
                        ///< estouro.
        const BubbleSettings&
            _cfg; ///< Referência persistente para as configurações globais.
};
