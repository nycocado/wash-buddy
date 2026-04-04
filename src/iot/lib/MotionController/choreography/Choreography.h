#pragma once

#include <Arduino.h>
#include <vector>

/**
 * @struct ChoreoStep
 * @brief Representa um "Passo" (Step) individual dentro de uma sequência de
 * movimento.
 */
struct ChoreoStep
{
        int targetAngle; ///< Ângulo de destino para o servo (0-180).
        float duration;  ///< Tempo de permanência/execução deste passo (em
                         ///< segundos).
};

/**
 * @class ChoreoSequence
 * @brief Motor de execução de sequências de movimentos (coreografias) para um
 * único eixo.
 *
 * Esta classe gerencia o tempo e a transição entre múltiplos ChoreoSteps,
 * permitindo criar movimentos complexos e repetitivos (loops) ou únicos
 * (one-shot).
 */
class ChoreoSequence
{
    public:
        /**
         * @brief Construtor do gerenciador de sequências.
         */
        ChoreoSequence();

        /**
         * @brief Define o conjunto de frames/passos que compõem a coreografia.
         * @param steps Vetor contendo os passos da sequência.
         */
        void setFrames(const std::vector<ChoreoStep>& steps);

        /**
         * @brief Inicia a reprodução da coreografia.
         * @param startDelay Atraso opcional (em segundos) antes de iniciar o
         * primeiro passo.
         * @param loop Se true, a sequência reinicia automaticamente ao chegar
         * no fim.
         */
        void play(float startDelay = 0.0f, bool loop = true);

        /**
         * @brief Interrompe a reprodução imediatamente.
         */
        void stop();

        /**
         * @brief Verifica se a coreografia está em execução ativa.
         * @return True se estiver rodando, False se estiver parada ou no delay
         * inicial.
         */
        bool isActive() const;

        /**
         * @brief Atualiza o temporizador interno e determina o passo atual.
         * Deve ser chamado a cada frame do loop principal.
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @return Ponteiro para o ChoreoStep atual se houver mudança de
         * comando, ou nullptr.
         */
        const ChoreoStep* update(float deltaTime);

    private:
        std::vector<ChoreoStep> _steps;
        size_t _currentStepIndex;
        float _timer;
        bool _isActive;
        bool _isLooping;
        bool _isFirstFrame;
};
