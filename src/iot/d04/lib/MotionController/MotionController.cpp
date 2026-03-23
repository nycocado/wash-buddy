#include "MotionController.h"
#include <math.h>

MotionController::MotionController(
    uint8_t pinArmL,
    uint8_t pinArmR,
    uint8_t pinHead,
    const MotionSettings& config
)
    : _config(config), _armL(
                           pinArmL,
                           config.defaultArmCenter,
                           config.defaultSpeed,
                           config.minArmAngle,
                           config.maxArmAngle
                       ),
      _armR(
          pinArmR,
          config.defaultArmCenter,
          config.defaultSpeed,
          config.minArmAngle,
          config.maxArmAngle
      ),
      _head(
          pinHead,
          config.defaultHeadCenter,
          config.headSpeed,
          config.minHeadAngle,
          config.maxHeadAngle
      )
{
}

/**
 * @section Ciclo de Vida e Atualização
 */

void MotionController::init()
{
    // A inicialização física dos pinos foi movida para o update()
    // para ser 100% não bloqueante (Staggered Startup).
    _initStage = 0;
    _lastInitTime = millis();
    Serial.println(F("[MotionController] Aguardando acionamento em cascata...")
    );
}

void MotionController::update(float deltaTime)
{
    // --- STAGGERED STARTUP (NÃO-BLOQUEANTE) ---
    // Ativa os servos um por um a cada 150ms para evitar pico de corrente
    // (Brownout) que ocorre ao ligar múltiplos motores simultaneamente.
    if (_initStage < 3)
    {
        if (millis() - _lastInitTime > 150)
        {
            if (_initStage == 0)
            {
                _armL.servo.attach(_armL.pin);
                // Força o primeiro pulso PWM para a posição central.
                // O delay de 150ms antes do próximo servo protege contra picos
                // de corrente.
                _armL.servo.write((int)round(_armL.currentAngle));
            }
            else if (_initStage == 1)
            {
                _armR.servo.attach(_armR.pin);
                _armR.servo.write((int)round(_armR.currentAngle));
            }
            else if (_initStage == 2)
            {
                _head.servo.attach(_head.pin);
                _head.servo.write((int)round(_head.currentAngle));
                Serial.println(F("[MotionController] Todos os servos online."));
            }

            _initStage++;
            _lastInitTime = millis();
        }
        return; // Sai do update enquanto não terminar de ligar os motores
    }

    // --- ATUALIZAÇÃO DE COREOGRAFIAS ---
    // Verifica se há passos pendentes em cada motor de sequência.
    // Se um novo passo for detectado, atualiza o alvo e a velocidade do motor.

    if (const ChoreoStep* step = _headChoreo.update(deltaTime))
    {
        moveHead(step->targetAngle, step->speed);
    }

    if (const ChoreoStep* step = _armLChoreo.update(deltaTime))
    {
        moveArmL(step->targetAngle, step->speed);
    }

    if (const ChoreoStep* step = _armRChoreo.update(deltaTime))
    {
        moveArmR(step->targetAngle, step->speed);
    }

    // --- GERENCIADOR DE INÉRCIA (INERTIA GATE) ---
    // Mesmo que o Slew Rate limite a aceleração, se múltiplos motores
    // começarem a se mover do zero absoluto no exato mesmo milissegundo,
    // a soma da corrente de arranque (Inrush) deles pode causar brownout.
    // O 'initiationTokenUsed' garante que apenas UM motor pode "quebrar a
    // inércia" por frame, desacoplando os picos elétricos sem afetar a fluidez
    // visual.
    bool initiationTokenUsed = false;

    // A física é processada individualmente para cada eixo de forma encapsulada
    if (updateAxisPhysics(_armL, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
    if (updateAxisPhysics(_armR, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
    if (updateAxisPhysics(_head, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
}

/**
 * @section Física e Interpolação (Controle de Corrente)
 */

bool MotionController::updateAxisPhysics(
    MotorAxis& axis,
    float deltaTime,
    bool initiationTokenUsed
)
{
    // Threshold para considerar que um eixo precisa sair do estado de repouso
    const float MOVEMENT_THRESHOLD = 0.5f;
    bool needsStart = !axis.isMoving && abs(axis.currentAngle - axis.targetAngle
                                        ) > MOVEMENT_THRESHOLD;
    bool tokenConsumed = false;

    // Se o eixo já estiver em movimento, ou se ele precisa iniciar E o token de
    // inércia deste frame ainda estiver disponível:
    if (axis.isMoving || (needsStart && !initiationTokenUsed))
    {
        if (needsStart)
        {
            tokenConsumed = true; // Consumiu o token de inércia deste frame
        }

        axis.isMoving = true;

        // Aplica a interpolação suave (Slew Rate Limited)
        axis.currentAngle = lerp(
            axis.currentAngle,
            axis.targetAngle,
            deltaTime,
            axis.speed,
            _config.maxVelocity
        );
        axis.servo.write((int)round(axis.currentAngle));

        // Verifica se chegou ao destino
        if (abs(axis.currentAngle - axis.targetAngle) <= MOVEMENT_THRESHOLD)
        {
            axis.isMoving = false;
        }
    }

    return tokenConsumed;
}

float MotionController::lerp(
    float current,
    float target,
    float deltaTime,
    float speed,
    float maxVelocity
)
{
    // Se a diferença for insignificante, finaliza o movimento no alvo exato
    if (abs(current - target) < 0.01f)
    {
        return target;
    }

    // Calcula o deslocamento ideal da curva exponencial (primeira ordem)
    // Isso cria um movimento orgânico que desacelera suavemente ao se aproximar
    // do alvo.
    float rawDisplacement =
        (target - current) * (1.0f - expf(-speed * deltaTime));

    // Calcula o deslocamento máximo permitido fisicamente (Slew Rate)
    // maxVelocity é dado em graus por segundo. Evita picos de corrente (Stall
    // current) ao impedir que o motor tente dar um passo muito grande em um
    // único frame.
    float maxDisplacement = maxVelocity * deltaTime;

    // Aplica o Clamping (Limitação) de aceleração
    if (rawDisplacement > maxDisplacement)
    {
        rawDisplacement = maxDisplacement;
    }
    else if (rawDisplacement < -maxDisplacement)
    {
        rawDisplacement = -maxDisplacement;
    }

    return current + rawDisplacement;
}

/**
 * @section Comandos de Movimento Individual
 */

void MotionController::moveArmL(int angle, float speed)
{
    _armL.setTarget(angle, speed > 0.0f ? speed : _config.defaultSpeed);
}

void MotionController::moveArmR(int angle, float speed)
{
    _armR.setTarget(angle, speed > 0.0f ? speed : _config.defaultSpeed);
}

void MotionController::moveHead(int angle, float speed)
{
    _head.setTarget(angle, speed > 0.0f ? speed : _config.headSpeed);
}

void MotionController::centerAll()
{
    // Velocidades bem baixas para um retorno suave e econômico
    moveArmL(_config.defaultArmCenter, 1.5f);
    moveArmR(_config.defaultArmCenter, 1.5f);
    moveHead(_config.defaultHeadCenter, 1.0f);
}

/**
 * @section Comandos de Gestos Prontos
 */

void MotionController::waveHands()
{
    moveArmL(130, 15.0f);
    moveArmR(50, 15.0f);
}

void MotionController::lookLeft() { moveHead(150, 6.0f); }

void MotionController::lookRight() { moveHead(30, 6.0f); }

/**
 * @section Gestão de Coreografias
 */

void MotionController::playHeadChoreography(
    const std::vector<ChoreoStep>& choreography,
    float startDelay,
    bool loop
)
{
    _headChoreo.setFrames(choreography);
    _headChoreo.play(startDelay, loop);
}

void MotionController::playArmLChoreography(
    const std::vector<ChoreoStep>& choreography,
    float startDelay,
    bool loop
)
{
    _armLChoreo.setFrames(choreography);
    _armLChoreo.play(startDelay, loop);
}

void MotionController::playArmRChoreography(
    const std::vector<ChoreoStep>& choreography,
    float startDelay,
    bool loop
)
{
    _armRChoreo.setFrames(choreography);
    _armRChoreo.play(startDelay, loop);
}

void MotionController::stopAllAnimations()
{
    _headChoreo.stop();
    _armLChoreo.stop();
    _armRChoreo.stop();
}
