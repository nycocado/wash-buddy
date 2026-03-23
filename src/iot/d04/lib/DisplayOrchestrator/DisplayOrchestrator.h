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
         * @brief Exibe um ícone de instrução por um tempo determinado.
         * Após o tempo expirar, o display volta a mostrar os olhos.
         * @param icon Ponteiro PROGMEM para o XBM.
         * @param durationMs Tempo em milissegundos (0 para fixo).
         */
        void
        showInstruction(const uint8_t* icon, unsigned long durationMs = 3000);

        /**
         * @brief Define o ícone de instrução a ser exibido (sem timeout).
         */
        void setInstructionIcon(
            const uint8_t* icon,
            uint8_t width = 64,
            uint8_t height = 64
        );

        /**
         * @brief Verifica se um ícone de instrução ou uma transição está ativa.
         */
        bool isInstructionActive() const
        {
            return (_currentInstructionIcon != nullptr) ||
                   (_pendingIcon != nullptr) ||
                   (_transitionState != TransitionState::IDLE);
        }

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

        /**
         * @brief Exibe um texto de debug no canto superior da tela.
         * @param text O texto a ser exibido. Deixe vazio ("") para ocultar.
         */
        void setDebugText(const String& text);

    private:
        const DisplayConfig _config;
        ExpressionEngine _eyes;
        ParticleSystem _animations;
        unsigned long _lastUpdate;
        String _debugText;

        const uint8_t* _currentInstructionIcon = nullptr;
        const uint8_t* _pendingIcon = nullptr; // Próximo ícone a ser mostrado
        uint8_t _instructionWidth = 64;
        uint8_t _instructionHeight = 64;
        unsigned long _instructionStartTime = 0;
        unsigned long _instructionEndTime = 0;

        // Controle de Transição
        enum class TransitionState
        {
            IDLE,
            CLOSING,
            OPENING
        };
        TransitionState _transitionState = TransitionState::IDLE;
        float _transitionProgress = 0.0f;
};
