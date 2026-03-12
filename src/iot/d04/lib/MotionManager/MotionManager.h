#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

struct MotionSettings
{
        int minAngle = 0;
        int maxAngle = 180;
        int defaultCenter = 90;
        float defaultSpeed = 10.0f;
        float headSpeed = 5.0f;
};

class MotionManager
{
    public:
        MotionManager(
            uint8_t pinArmL,
            uint8_t pinArmR,
            uint8_t pinHead,
            const MotionSettings& config = MotionSettings()
        );

        void init();
        void update(float deltaTime);
        void moveArmL(int angle, float speed = -1.0f);
        void moveArmR(int angle, float speed = -1.0f);
        void moveHead(int angle, float speed = -1.0f);
        void centerAll();
        void waveHands();
        void lookLeft();
        void lookRight();

    private:
        Servo _servoArmL;
        Servo _servoArmR;
        Servo _servoHead;
        uint8_t _pinL, _pinR, _pinH;
        const MotionSettings _config;
        float _currArmL, _targetArmL, _speedArmL;
        float _currArmR, _targetArmR, _speedArmR;
        float _currHead, _targetHead, _speedHead;
        
        float lerp(float current, float target, float deltaTime, float speed);
};
