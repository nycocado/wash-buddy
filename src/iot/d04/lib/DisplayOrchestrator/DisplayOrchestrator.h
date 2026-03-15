#pragma once

#include "Common.h"
#include "ExpressionEngine.h"
#include "ParticleSystem.h"
#include <Arduino.h>

/**
 * @brief Configurações básicas de hardware e tempo do display.
 */
struct DisplayConfig
{
        uint8_t screenWidth = 128;   ///< Largura da tela em pixels
        uint8_t screenHeight = 64;   ///< Altura da tela em pixels
        uint32_t frameInterval = 16; ///< Intervalo entre quadros (ms) ~60fps
};

/**
 * @class DisplayOrchestrator
 * @brief Orquestrador central de toda a saída visual no OLED.
 *
 * Esta classe é responsável por sincronizar o desenho das expressões
 * (ExpressionEngine) e dos sistemas de partículas (ParticleSystem) em um único
 * buffer da U8g2, garantindo que a atualização da tela seja fluida e sem
 * flickering.
 */
class DisplayOrchestrator
{
    public:
        /**
         * @brief Construtor do orquestrador.
         * @param config Estrutura com as definições de tela.
         */
        DisplayOrchestrator(const DisplayConfig& config = DisplayConfig());

        /**
         * @brief Inicializa o hardware do display e os sub-sistemas.
         * Configura os pinos I2C e prepara os motores de animação internos.
         */
        void init();

        /**
         * @brief Atualiza a lógica de animação e renderiza o próximo quadro.
         * Deve ser chamado continuamente no loop principal (ou task dedicada).
         */
        void update();

        /**
         * @brief Define o humor/expressão atual.
         * @param mood Um dos valores do enum eEmotions (ex: Happy, Angry).
         */
        void setEyeMood(eEmotions mood);

        /**
         * @brief Ativa ou desativa o modo de espera dos olhos.
         * @param active True para ativar o movimento aleatório, False para
         * fixar.
         */
        void setEyeIdleMode(bool active);

        /**
         * @brief Força uma animação de piscar imediata.
         */
        void blinkEyes();

        /**
         * @brief Direciona o olhar para uma coordenada específica.
         * @param x Posição horizontal (-1.0 esquerda, 1.0 direita).
         * @param y Posição vertical (-1.0 baixo, 1.0 cima).
         */
        void lookAt(float x, float y);

        /**
         * @brief Ativa uma animação curta de confusão/dúvida.
         */
        void playConfused();

        /**
         * @brief Ativa uma animação curta de felicidade/comemoração.
         */
        void playHappy();

        /**
         * @brief Controla a visibilidade dos olhos na tela.
         * @param visible True para desenhar, False para ocultar.
         */
        void setEyesVisible(bool visible);

        /**
         * @brief Define o efeito de partículas atmosféricas ativo.
         * @param type Tipo de efeito (ex: BUBBLES, RAIN, DRIP).
         */
        void setParticleEffect(EffectType type);

        /**
         * @brief Define a probabilidade de nascimento de novas partículas.
         * @param chance Valor de 0 a 100.
         */
        void setParticleSpawnChance(int chance);

        /**
         * @brief Limita a quantidade máxima de partículas simultâneas.
         * @param max Quantidade máxima permitida.
         */
        void setMaxParticles(int max);

        /**
         * @brief Limpa manualmente o buffer interno de desenho.
         */
        void clear();

        /**
         * @brief Transmite manualmente o buffer consolidado para o hardware.
         */
        void render();

        /**
         * @brief Coloca o controlador do display em modo Power Save.
         */
        void prepareForSleep();

        /**
         * @brief Acorda o controlador do display.
         */
        void wakeUp();

    private:
        const DisplayConfig _config;
        ExpressionEngine _eyes;
        ParticleSystem _animations;
        unsigned long _lastUpdate;
};
