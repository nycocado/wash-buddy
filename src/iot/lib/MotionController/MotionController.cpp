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
                           config.minArmAngle,
                           config.maxArmAngle,
                           config.invertArmL
                       ),
      _armR(
          pinArmR,
          config.defaultArmCenter,
          config.minArmAngle,
          config.maxArmAngle,
          config.invertArmR
      ),
      _head(
          pinHead,
          config.defaultHeadCenter,
          config.minHeadAngle,
          config.maxHeadAngle,
          config.invertHead
      )
{
}

void MotionController::init()
{
    _initStage = 0;
    _lastInitTime = millis();
    Serial.println(F("[MotionController] Aguardando acionamento em cascata...")
    );
}

void MotionController::update(float deltaTime)
{
    if (_initStage < 4)
    {
        unsigned long now = millis();
        if (now - _lastInitTime > 200)
        {
            switch (_initStage)
            {
                case 0:
                    _armL.servo.attach(_armL.pin);
                    _armL.servo.write((int)round(_armL.currentAngle));
                    break;
                case 1:
                    _armR.servo.attach(_armR.pin);
                    _armR.servo.write((int)round(_armR.currentAngle));
                    break;
                case 2:
                    _head.servo.attach(_head.pin);
                    _head.servo.write((int)round(_head.currentAngle));
                    break;
                case 3:
                    break;
            }
            _initStage++;
            _lastInitTime = now;
        }
        return;
    }

    if (const ChoreoStep* step = _headChoreo.update(deltaTime))
    {
        moveHead(step->targetAngle, step->duration);
    }
    if (const ChoreoStep* step = _armLChoreo.update(deltaTime))
    {
        moveArmL(step->targetAngle, step->duration);
    }
    if (const ChoreoStep* step = _armRChoreo.update(deltaTime))
    {
        moveArmR(step->targetAngle, step->duration);
    }

    bool initiationTokenUsed = false;

    if (updateAxisPhysics(_armL, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
    if (updateAxisPhysics(_armR, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
    if (updateAxisPhysics(_head, deltaTime, initiationTokenUsed))
        initiationTokenUsed = true;
}

bool MotionController::updateAxisPhysics(
    MotorAxis& axis,
    float deltaTime,
    bool initiationTokenUsed
)
{
    bool tokenConsumed = false;

    if (axis.isMoving)
    {
        // Precisamos do token de inicialização apenas se estamos assumindo
        // inércia inicial pesada, mas em movimentos contínuos não bloqueamos
        // por token. Aqui mantemos a estrutura de token para não dar picos
        // elétricos.
        if (axis.elapsedTime == 0.0f && !initiationTokenUsed)
        {
            tokenConsumed = true;
        }
        else if (axis.elapsedTime == 0.0f && initiationTokenUsed)
        {
            return false; // Wait for next frame to start moving this servo
        }

        if (axis.moveDuration <= 0.001f)
        {
            // Movimento instantâneo (Teleporte)
            axis.currentAngle = axis.targetAngle;
            axis.isMoving = false;
        }
        else
        {
            axis.elapsedTime += deltaTime;

            float progress = axis.elapsedTime / axis.moveDuration;

            if (progress >= 1.0f)
            {
                progress = 1.0f;
                axis.isMoving = false;
            }

            axis.currentAngle =
                easeSmoothStep(axis.startAngle, axis.targetAngle, progress);
        }

        float writeAngle =
            axis.isInverted ? 180.0f - axis.currentAngle : axis.currentAngle;
        axis.servo.write((int)round(writeAngle));
    }

    return tokenConsumed;
}

float MotionController::easeSmoothStep(float start, float target, float progress)
{
    if (progress <= 0.0f)
        return start;
    if (progress >= 1.0f)
        return target;

    // Smoothstep (Ease-in / Ease-out)
    // f(t) = t^2 * (3 - 2t)
    float smoothProgress = progress * progress * (3.0f - 2.0f * progress);

    return start + (target - start) * smoothProgress;
}

void MotionController::moveArmL(int angle, float duration)
{
    _armL.setTarget(
        angle, duration < 0.0f ? _config.defaultDuration : duration
    );
}

void MotionController::moveArmR(int angle, float duration)
{
    _armR.setTarget(
        angle, duration < 0.0f ? _config.defaultDuration : duration
    );
}

void MotionController::moveHead(int angle, float duration)
{
    _head.setTarget(
        angle, duration < 0.0f ? _config.defaultDuration : duration
    );
}

void MotionController::centerAll()
{
    moveArmL(_config.defaultArmCenter);
    moveArmR(_config.defaultArmCenter);
    moveHead(_config.defaultHeadCenter);
}

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
