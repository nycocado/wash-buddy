#pragma once

#include "Face.h"
#include <Arduino.h>

/**
 * @brief Configurações básicas para o comportamento da face/expressões.
 */
struct ExpressionConfig
{
        int blinkInterval = 3500; ///< Intervalo médio entre piscadas (ms)
        uint16_t eyeSize = 40;    ///< Tamanho base dos olhos em pixels
};

/**
 * @class ExpressionEngine
 * @brief Gerencia a renderização e o comportamento expressivo dos olhos do
 * robô.
 *
 * Esta classe encapsula a lógica da biblioteca esp32-eyes, permitindo
 * controlar emoções, direção do olhar e animações automáticas (piscar/olhar
 * aleatório). Ela abstrai a complexidade do hardware e fornece uma interface
 * semântica para o GameController.
 */
class ExpressionEngine
{
    public:
        ExpressionEngine();
        ~ExpressionEngine();

        /**
         * @brief Inicializa o motor de renderização de olhos/expressões.
         * @param width Largura da tela em pixels.
         * @param height Altura da tela em pixels.
         * @param config Estrutura de configuração (opcional).
         */
        void begin(
            uint16_t width,
            uint16_t height,
            const ExpressionConfig& config = ExpressionConfig()
        );

        /**
         * @brief Atualiza os estados internos de animação e comportamentos.
         */
        void update();

        /**
         * @brief Desenha os olhos no buffer atual da U8g2.
         */
        void draw();

        /**
         * @brief Alterna a visibilidade dos olhos na tela.
         * @param visible True para mostrar, False para ocultar.
         */
        void setVisibility(bool visible);

        /**
         * @brief Verifica se os olhos estão visíveis.
         * @return Estado de visibilidade.
         */
        bool isVisible() const { return _isVisible; }

        /**
         * @brief Altera a emoção/expressão atual.
         * @param mood Enum eEmotions (ex: Focused, Happy, Sad).
         */
        void setMood(eEmotions mood);

        /**
         * @brief Ativa/desativa comportamentos automáticos de idle.
         * @param active Se true, o robô pisca e olha ao redor sozinho.
         */
        void setIdleMode(bool active);

        /**
         * @brief Executa um ciclo de piscada manual imediato.
         */
        void blink();

        /**
         * @brief Direciona o olhar para coordenadas normalizadas.
         * @param x Posição horizontal (-1.0 a 1.0).
         * @param y Posição vertical (-1.0 a 1.0).
         */
        void lookAt(float x, float y);

        /**
         * @brief Toca animação de confusão (olhar cético).
         */
        void playConfused();

        /**
         * @brief Toca animação de alegria (olhar brilhante).
         */
        void playHappy();

    private:
        Face* _face;     ///< Ponteiro para a implementação física da face
        bool _isVisible; ///< Flag de controle de renderização
};
