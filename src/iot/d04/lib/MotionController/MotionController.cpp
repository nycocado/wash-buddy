#include "MotionController.h"
#include <math.h>

MotionController::MotionController(
    uint8_t pinArmL,
    uint8_t pinArmR,
    uint8_t pinHead,
    const MotionSettings& config
)
    : _pinL(pinArmL), _pinR(pinArmR), _pinH(pinHead), _config(config),
      _currArmL(_config.defaultArmCenter),
      _targetArmL(_config.defaultArmCenter), _speedArmL(_config.defaultSpeed),
      _currArmR(_config.defaultArmCenter),
      _targetArmR(_config.defaultArmCenter), _speedArmR(_config.defaultSpeed),
      _currHead(_config.defaultHeadCenter),
      _targetHead(_config.defaultHeadCenter), _speedHead(_config.headSpeed)
{
}

void MotionController::init()
{
    // Vincula os objetos Servo aos pinos físicos do ESP32
    _servoArmL.attach(_pinL);
    _servoArmR.attach(_pinR);
    _servoHead.attach(_pinH);

    // Move imediatamente para a posição de repouso configurada
    _servoArmL.write(_config.defaultArmCenter);
    _servoArmR.write(_config.defaultArmCenter);
    _servoHead.write(_config.defaultHeadCenter);
}

void MotionController::update(float deltaTime)
{
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

    // --- INTERPOLAÇÃO DE MOVIMENTO (LERP) ---
    // Em vez de mover o servo instantaneamente, calculamos uma posição
    // intermediária baseada no tempo decorrido (deltaTime). Isso cria
    // um movimento fluido e orgânico, similar ao de um ser vivo.

    // Braço Esquerdo
    _currArmL = lerp(_currArmL, _targetArmL, deltaTime, _speedArmL);
    _servoArmL.write((int)round(_currArmL));

    // Braço Direito
    _currArmR = lerp(_currArmR, _targetArmR, deltaTime, _speedArmR);
    _servoArmR.write((int)round(_currArmR));

    // Cabeça
    _currHead = lerp(_currHead, _targetHead, deltaTime, _speedHead);
    _servoHead.write((int)round(_currHead));
}

float MotionController::lerp(
    float current,
    float target,
    float deltaTime,
    float speed
)
{
    // Se a diferença for insignificante, finaliza o movimento no alvo exato
    if (abs(current - target) < 0.01f)
    {
        return target;
    }

    // Interpolação Exponencial: o movimento começa rápido e suaviza ao chegar
    // no alvo
    return current + (target - current) * (1.0f - expf(-speed * deltaTime));
}

void MotionController::moveArmL(int angle, float speed)
{
    _targetArmL =
        (float)constrain(angle, _config.minArmAngle, _config.maxArmAngle);
    _speedArmL = speed < 0.0f ? _config.defaultSpeed : speed;
}

void MotionController::moveArmR(int angle, float speed)
{
    _targetArmR =
        (float)constrain(angle, _config.minArmAngle, _config.maxArmAngle);
    _speedArmR = speed < 0.0f ? _config.defaultSpeed : speed;
}

void MotionController::moveHead(int angle, float speed)
{
    _targetHead =
        (float)constrain(angle, _config.minHeadAngle, _config.maxHeadAngle);
    _speedHead = speed < 0.0f ? _config.headSpeed : speed;
}

void MotionController::centerAll()
{
    moveArmL(_config.defaultArmCenter, 8.0f);
    moveArmR(_config.defaultArmCenter, 8.0f);
    moveHead(_config.defaultHeadCenter, 4.0f);
}

void MotionController::waveHands()
{
    moveArmL(160, 15.0f);
    moveArmR(20, 15.0f);
}

void MotionController::lookLeft() { moveHead(150, 6.0f); }

void MotionController::lookRight() { moveHead(30, 6.0f); }

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
