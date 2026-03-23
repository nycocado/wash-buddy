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
        int defaultArmCenter = 90;  ///< Posição de repouso dos braços
        float defaultSpeed = 10.0f; ///< Velocidade padrão de interpolação
        float headSpeed = 5.0f;     ///< Velocidade padrão da cabeça
        float maxVelocity = 150.0f; ///< Limite de graus por segundo (Slew Rate)
                                    ///< para evitar picos de corrente
};

/**
 * @brief Estrutura interna para encapsular a física e o estado de um motor.
 */
struct MotorAxis
{
        Servo servo;
        uint8_t pin;
        float currentAngle;
        float targetAngle;
        float speed;
        int minAngle;
        int maxAngle;
        bool isMoving;

        MotorAxis(
            uint8_t p_pin,
            int startAngle,
            float startSpeed,
            int minA,
            int maxA
        )
            : pin(p_pin), currentAngle(startAngle), targetAngle(startAngle),
              speed(startSpeed), minAngle(minA), maxAngle(maxA), isMoving(false)
        {
        }

        void setTarget(int angle, float p_speed)
        {
            targetAngle = (float)constrain(angle, minAngle, maxAngle);
            if (p_speed > 0.0f)
            {
                speed = p_speed;
            }
        }
};

/**
 * @class MotionController
 * @brief Controlador físico de atuadores (Servos).
 *
 * Esta classe gerencia 3 servos (Cabeça, Braço Esquerdo, Braço Direito)
 * utilizando suavização via interpolação exponencial (lerp) limitando
 * a aceleração máxima para proteger a fonte de alimentação.
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
         * @brief Inicializa os servos e prepara o acionamento em cascata.
         * Configura o estágio inicial de ativação para evitar picos de
         * corrente.
         */
        void init();

        /**
         * @brief Atualiza a lógica de movimento e física dos servos.
         * Deve ser chamado continuamente no loop principal para processar
         * as interpolações e coreografias.
         * @param deltaTime Tempo decorrido desde o último quadro (em segundos).
         */
        void update(float deltaTime);

        /**
         * @brief Move o braço esquerdo para um ângulo específico com velocidade
         * controlada.
         * @param angle Ângulo de destino (0-180).
         * @param speed Velocidade de interpolação (negativo para usar default).
         */
        void moveArmL(int angle, float speed = -1.0f);

        /**
         * @brief Move o braço direito para um ângulo específico com velocidade
         * controlada.
         * @param angle Ângulo de destino (0-180).
         * @param speed Velocidade de interpolação (negativo para usar default).
         */
        void moveArmR(int angle, float speed = -1.0f);

        /**
         * @brief Move a cabeça para um ângulo específico com velocidade
         * controlada.
         * @param angle Ângulo de destino (0-180).
         * @param speed Velocidade de interpolação (negativo para usar default).
         */
        void moveHead(int angle, float speed = -1.0f);

        /**
         * @brief Retorna todos os servos para suas posições de repouso de forma
         * suave.
         */
        void centerAll();

        /**
         * @brief Executa um gesto pré-definido de aceno com as mãos.
         */
        void waveHands();

        /**
         * @brief Vira a cabeça suavemente para a esquerda.
         */
        void lookLeft();

        /**
         * @brief Vira a cabeça suavemente para a direita.
         */
        void lookRight();

        /**
         * @brief Inicia uma sequência de passos (coreografia) na cabeça.
         * @param choreography Vetor de passos ChoreoStep.
         * @param startDelay Atraso inicial opcional antes do primeiro passo.
         * @param loop Se a coreografia deve se repetir infinitamente.
         */
        void playHeadChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Inicia uma sequência de passos (coreografia) no braço
         * esquerdo.
         * @param choreography Vetor de passos ChoreoStep.
         * @param startDelay Atraso inicial opcional antes do primeiro passo.
         * @param loop Se a coreografia deve se repetir infinitamente.
         */
        void playArmLChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Inicia uma sequência de passos (coreografia) no braço direito.
         * @param choreography Vetor de passos ChoreoStep.
         * @param startDelay Atraso inicial opcional antes do primeiro passo.
         * @param loop Se a coreografia deve se repetir infinitamente.
         */
        void playArmRChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Verifica se a cabeça está atualmente executando uma
         * coreografia.
         * @return true se uma animação estiver ativa, false caso contrário.
         */
        bool isHeadAnimActive() const { return _headChoreo.isActive(); }

        /**
         * @brief Verifica se o braço esquerdo está atualmente executando uma
         * coreografia.
         * @return true se uma animação estiver ativa, false caso contrário.
         */
        bool isArmLAnimActive() const { return _armLChoreo.isActive(); }

        /**
         * @brief Verifica se o braço direito está atualmente executando uma
         * coreografia.
         * @return true se uma animação estiver ativa, false caso contrário.
         */
        bool isArmRAnimActive() const { return _armRChoreo.isActive(); }

        /**
         * @brief Interrompe imediatamente todas as coreografias em curso.
         */
        void stopAllAnimations();

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
         * @return true se o eixo estava parado e "consumiu" o token de
         * iniciação.
         */
        bool updateAxisPhysics(
            MotorAxis& axis,
            float deltaTime,
            bool initiationTokenUsed
        );

        /**
         * @brief Calcula a interpolação com limitação de taxa de variação (Slew
         * Rate).
         */
        float lerp(
            float current,
            float target,
            float deltaTime,
            float speed,
            float maxVelocity
        );
};
