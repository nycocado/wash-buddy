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
        int minHeadAngle = 0;       ///< Ângulo mínimo da cabeça
        int maxHeadAngle = 180;     ///< Ângulo máximo da cabeça
        int minArmAngle = 0;        ///< Ângulo mínimo dos braços
        int maxArmAngle = 180;      ///< Ângulo máximo dos braços
        int defaultHeadCenter = 90; ///< Posição central da cabeça
        int defaultArmCenter = 0;   ///< Posição de repouso dos braços
        float defaultSpeed = 10.0f; ///< Velocidade padrão de interpolação
        float headSpeed = 5.0f;     ///< Velocidade padrão da cabeça
};

/**
 * @class MotionController
 * @brief Controlador físico de atuadores (Servos).
 *
 * Esta classe gerencia 3 servos (Cabeça, Braço Esquerdo, Braço Direito)
 * utilizando suavização via interpolação exponencial (lerp) e suporte a
 * reprodução de sequências complexas de movimentos (Timelines).
 */
class MotionController
{
    public:
        /**
         * @brief Construtor do gerenciador de movimentos.
         * @param pinArmL Pino PWM do braço esquerdo.
         * @param pinArmR Pino PWM do braço direito.
         * @param pinHead Pino PWM da cabeça.
         * @param config Configurações de limites e velocidades.
         */
        MotionController(
            uint8_t pinArmL,
            uint8_t pinArmR,
            uint8_t pinHead,
            const MotionSettings& config = MotionSettings()
        );

        /**
         * @brief Inicializa os servos e os move para a posição inicial.
         */
        void init();

        /**
         * @brief Atualiza a posição de todos os servos baseada no deltaTime.
         */
        void update(float deltaTime);

        /**
         * @brief Move o braço esquerdo para um ângulo específico.
         */
        void moveArmL(int angle, float speed = -1.0f);

        /**
         * @brief Move o braço direito para um ângulo específico.
         */
        void moveArmR(int angle, float speed = -1.0f);

        /**
         * @brief Move a cabeça para um ângulo específico.
         */
        void moveHead(int angle, float speed = -1.0f);

        /**
         * @brief Retorna todos os servos para suas posições de repouso.
         */
        void centerAll();

        /**
         * @brief Executa um gesto pré-definido de aceno com as mãos.
         */
        void waveHands();

        /**
         * @brief Vira a cabeça para a esquerda.
         */
        void lookLeft();

        /**
         * @brief Vira a cabeça para a direita.
         */
        void lookRight();

        /**
         * @brief Reproduz uma coreografia na cabeça.
         */
        void playHeadChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Reproduz uma coreografia no braço esquerdo.
         */
        void playArmLChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Reproduz uma coreografia no braço direito.
         */
        void playArmRChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        bool isHeadAnimActive() const { return _headChoreo.isActive(); }
        bool isArmLAnimActive() const { return _armLChoreo.isActive(); }
        bool isArmRAnimActive() const { return _armRChoreo.isActive(); }

        /**
         * @brief Interrompe todas as coreografias em curso.
         */
        void stopAllAnimations();

    private:
        Servo _servoArmL;
        Servo _servoArmR;
        Servo _servoHead;
        uint8_t _pinL, _pinR, _pinH;
        const MotionSettings _config;
        float _currArmL, _targetArmL, _speedArmL;
        float _currArmR, _targetArmR, _speedArmR;
        float _currHead, _targetHead, _speedHead;

        ChoreoSequence _headChoreo;
        ChoreoSequence _armLChoreo;
        ChoreoSequence _armRChoreo;

        /**
         * @brief Calcula a interpolação suave entre dois valores.
         */
        float lerp(float current, float target, float deltaTime, float speed);
};
