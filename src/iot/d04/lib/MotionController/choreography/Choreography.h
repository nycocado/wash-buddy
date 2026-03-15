#pragma once

#include <Arduino.h>
#include <vector>

/**
 * @brief Representa um "Passo" (Step) de uma coreografia de movimento.
 */
struct ChoreoStep
{
        int targetAngle; ///< Ângulo de destino para o servo
        float duration;  ///< Quanto tempo (em segundos) o passo deve durar
        float speed;     ///< Velocidade de interpolação para este passo
};

/**
 * @class ChoreoSequence
 * @brief Gerenciador de sequências de passos (coreografia) para um único servo.
 */
class ChoreoSequence
{
    public:
        ChoreoSequence();

        /**
         * @brief Define a lista de passos da coreografia.
         * @param steps Vetor de passos.
         */
        void setFrames(const std::vector<ChoreoStep>& steps);

        /**
         * @brief Inicia a reprodução da sequência.
         * @param startDelay Atraso inicial opcional (s).
         * @param loop Se a coreografia deve repetir indefinidamente.
         */
        void play(float startDelay = 0.0f, bool loop = true);

        /**
         * @brief Para a reprodução imediatamente.
         */
        void stop();

        /**
         * @brief Verifica se a coreografia está sendo reproduzida.
         */
        bool isActive() const;

        /**
         * @brief Atualiza o timer da coreografia.
         * @return O passo atual a ser executado, ou nullptr se nada mudou.
         */
        const ChoreoStep* update(float deltaTime);

    private:
        std::vector<ChoreoStep> _steps;
        size_t _currentStepIndex;
        float _timer;
        bool _isActive;
        bool _isLooping;
};
