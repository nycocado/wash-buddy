#pragma once

#include "Choreography.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <vector>

/**
 * @brief Configurações de limites e velocidades dos servos.
 */
struct MotionSettings
{
        int minHeadAngle = 0;         ///< Ângulo mínimo da cabeça
        int maxHeadAngle = 180;       ///< Ângulo máximo da cabeça
        int minArmAngle = 0;          ///< Ângulo mínimo dos braços
        int maxArmAngle = 180;        ///< Ângulo máximo dos braços
        int defaultHeadCenter = 90;   ///< Posição central da cabeça
        int defaultArmCenter = 90;    ///< Posição de repouso dos braços
        float defaultDuration = 1.0f; ///< Duração padrão (segundos)
        bool invertArmL = false;      ///< Inversão lógica do braço esquerdo
        bool invertArmR = true;       ///< Inversão lógica do braço direito
        bool invertHead = false;      ///< Inversão lógica da cabeça
};

/**
 * @brief Estrutura interna para encapsular a física e o estado de um motor.
 */
struct MotorAxis
{
        Servo servo;
        uint8_t pin;
        float currentAngle;

        float startAngle;
        float targetAngle;
        float moveDuration;
        float elapsedTime;

        int minAngle;
        int maxAngle;
        bool isMoving;
        bool isInverted; ///< Se true, inverte o sinal (180 - angulo)

        MotorAxis(
            uint8_t p_pin,
            int initialAngle,
            int minA,
            int maxA,
            bool inverted = false
        )
            : pin(p_pin), currentAngle(initialAngle), startAngle(initialAngle),
              targetAngle(initialAngle), moveDuration(0.0f), elapsedTime(0.0f),
              minAngle(minA), maxAngle(maxA), isMoving(false),
              isInverted(inverted)
        {
        }

        /**
         * @brief Define o novo alvo de movimento.
         * @param angle Ângulo desejado.
         * @param duration Tempo para completar o movimento (segundos).
         */
        void setTarget(int angle, float duration = 1.0f)
        {
            startAngle = currentAngle;
            targetAngle = (float)constrain(angle, minAngle, maxAngle);
            moveDuration = duration;
            elapsedTime = 0.0f;
            isMoving = true;
        }
};

/**
 * @class MotionController
 * @brief Controlador físico de atuadores (Servos) baseado em tempo.
 */
class MotionController
{
    public:
        /**
         * @brief Construtor do gerenciador de movimentos.
         * @param pinArmL Pino do braço esquerdo.
         * @param pinArmR Pino do braço direito.
         * @param pinHead Pino da cabeça.
         * @param config Estrutura de configurações físicas.
         */
        MotionController(
            uint8_t pinArmL,
            uint8_t pinArmR,
            uint8_t pinHead,
            const MotionSettings& config = MotionSettings()
        );

        /** @brief Inicializa os servos e prepara o acionamento em cascata. */
        void init();

        /**
         * @brief Atualiza a física dos servos.
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         */
        void update(float deltaTime);

        /**
         * @brief Move o braço esquerdo.
         * @param angle Ângulo de destino.
         * @param duration Duração do movimento (segundos).
         */
        void moveArmL(int angle, float duration = -1.0f);

        /**
         * @brief Move o braço direito.
         * @param angle Ângulo de destino.
         * @param duration Duração do movimento (segundos).
         */
        void moveArmR(int angle, float duration = -1.0f);

        /**
         * @brief Move a cabeça.
         * @param angle Ângulo de destino.
         * @param duration Duração do movimento (segundos).
         */
        void moveHead(int angle, float duration = -1.0f);

        /** @brief Retorna todos os servos para suas posições de repouso. */
        void centerAll();

        /**
         * @brief Inicia uma coreografia na cabeça.
         * @param choreography Vetor de passos.
         * @param startDelay Atraso inicial (segundos).
         * @param loop Se deve repetir.
         */
        void playHeadChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Inicia uma coreografia no braço esquerdo.
         * @param choreography Vetor de passos.
         * @param startDelay Atraso inicial (segundos).
         * @param loop Se deve repetir.
         */
        void playArmLChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Inicia uma coreografia no braço direito.
         * @param choreography Vetor de passos.
         * @param startDelay Atraso inicial (segundos).
         * @param loop Se deve repetir.
         */
        void playArmRChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /** @brief Interrompe todas as animações ativas. */
        void stopAllAnimations();

        /**
         * @brief Calcula a interpolação suave (Smoothstep).
         * @param start Ângulo inicial.
         * @param target Ângulo de destino.
         * @param progress Progresso linear de 0.0 a 1.0.
         * @return Ângulo interpolado na curva suave.
         */
        static float
        easeSmoothStep(float start, float target, float progress);

    private:
        const MotionSettings _config;

        MotorAxis _armL;
        MotorAxis _armR;
        MotorAxis _head;

        ChoreoSequence _headChoreo;
        ChoreoSequence _armLChoreo;
        ChoreoSequence _armRChoreo;

        uint8_t _initStage = 0;
        unsigned long _lastInitTime = 0;

        /**
         * @brief Processa a física de um eixo específico.
         * @return True se um token de inicialização foi consumido.
         */
        bool updateAxisPhysics(
            MotorAxis& axis,
            float deltaTime,
            bool initiationTokenUsed
        );
};
