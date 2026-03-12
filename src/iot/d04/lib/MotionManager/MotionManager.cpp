#include "MotionManager.h"
#include <math.h>

MotionManager::MotionManager(
    uint8_t pinArmL,
    uint8_t pinArmR,
    uint8_t pinHead,
    const MotionSettings& config
)
    : _pinL(pinArmL), _pinR(pinArmR), _pinH(pinHead), _config(config),
      _currArmL(_config.defaultCenter), _targetArmL(_config.defaultCenter),
      _speedArmL(_config.defaultSpeed), _currArmR(_config.defaultCenter),
      _targetArmR(_config.defaultCenter), _speedArmR(_config.defaultSpeed),
      _currHead(_config.defaultCenter), _targetHead(_config.defaultCenter),
      _speedHead(_config.headSpeed)
{
}

void MotionManager::init()
{
    _servoArmL.attach(_pinL);
    _servoArmR.attach(_pinR);
    _servoHead.attach(_pinH);

    _servoArmL.write(_config.defaultCenter);
    _servoArmR.write(_config.defaultCenter);
    _servoHead.write(_config.defaultCenter);
}

void MotionManager::update(float deltaTime)
{
    _currArmL = lerp(_currArmL, _targetArmL, deltaTime, _speedArmL);
    _servoArmL.write((int)round(_currArmL));

    _currArmR = lerp(_currArmR, _targetArmR, deltaTime, _speedArmR);
    _servoArmR.write((int)round(_currArmR));

    _currHead = lerp(_currHead, _targetHead, deltaTime, _speedHead);
    _servoHead.write((int)round(_currHead));
}

float MotionManager::lerp(
    float current,
    float target,
    float deltaTime,
    float speed
)
{
    if (abs(current - target) < 0.1f)
        return target;
    return current + (target - current) * (1.0f - expf(-speed * deltaTime));
}

void MotionManager::moveArmL(int angle, float speed)
{
    _targetArmL = (float)constrain(angle, _config.minAngle, _config.maxAngle);
    _speedArmL = speed < 0.0f ? _config.defaultSpeed : speed;
}

void MotionManager::moveArmR(int angle, float speed)
{
    _targetArmR = (float)constrain(angle, _config.minAngle, _config.maxAngle);
    _speedArmR = speed < 0.0f ? _config.defaultSpeed : speed;
}

void MotionManager::moveHead(int angle, float speed)
{
    _targetHead = (float)constrain(angle, _config.minAngle, _config.maxAngle);
    _speedHead = speed < 0.0f ? _config.headSpeed : speed;
}

void MotionManager::centerAll()
{
    moveArmL(_config.defaultCenter, 8.0f);
    moveArmR(_config.defaultCenter, 8.0f);
    moveHead(_config.defaultCenter, 4.0f);
}

void MotionManager::waveHands()
{
    moveArmL(160, 15.0f);
    moveArmR(20, 15.0f);
}

void MotionManager::lookLeft() { moveHead(150, 6.0f); }

void MotionManager::lookRight() { moveHead(30, 6.0f); }
