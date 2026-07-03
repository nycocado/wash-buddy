#pragma once

#include "Choreography.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <vector>

/**
 * @brief Servo limit and speed settings.
 */
struct MotionSettings
{
        int minHeadAngle = 0;         ///< Minimum head angle
        int maxHeadAngle = 180;       ///< Maximum head angle
        int minArmAngle = 0;          ///< Minimum arm angle
        int maxArmAngle = 180;        ///< Maximum arm angle
        int defaultHeadCenter = 90;   ///< Head's center position
        int defaultArmCenter = 90;    ///< Arms' rest position
        float defaultDuration = 1.0f; ///< Default duration (seconds)
        bool invertArmL = false;      ///< Logical inversion of the left arm
        bool invertArmR = true;       ///< Logical inversion of the right arm
        bool invertHead = false;      ///< Logical inversion of the head
};

/**
 * @brief Internal structure encapsulating one motor's physics and state.
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
        bool isInverted; ///< If true, inverts the signal (180 - angle)

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
         * @brief Sets the new movement target.
         * @param angle Desired angle.
         * @param duration Time to complete the movement (seconds).
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
 * @brief Time-based physical controller for actuators (servos).
 */
class MotionController
{
    public:
        /**
         * @brief Constructor for the motion manager.
         * @param pinArmL Left arm pin.
         * @param pinArmR Right arm pin.
         * @param pinHead Head pin.
         * @param config Physical settings structure.
         */
        MotionController(
            uint8_t pinArmL,
            uint8_t pinArmR,
            uint8_t pinHead,
            const MotionSettings& config = MotionSettings()
        );

        /** @brief Initializes the servos and prepares the cascading power-up.
         */
        void init();

        /**
         * @brief Updates the servo physics.
         * @param deltaTime Time elapsed since the last frame (seconds).
         */
        void update(float deltaTime);

        /**
         * @brief Moves the left arm.
         * @param angle Destination angle.
         * @param duration Movement duration (seconds).
         */
        void moveArmL(int angle, float duration = -1.0f);

        /**
         * @brief Moves the right arm.
         * @param angle Destination angle.
         * @param duration Movement duration (seconds).
         */
        void moveArmR(int angle, float duration = -1.0f);

        /**
         * @brief Moves the head.
         * @param angle Destination angle.
         * @param duration Movement duration (seconds).
         */
        void moveHead(int angle, float duration = -1.0f);

        /** @brief Returns every servo to its rest position. */
        void centerAll();

        /**
         * @brief Starts a choreography on the head.
         * @param choreography Vector of steps.
         * @param startDelay Initial delay (seconds).
         * @param loop Whether it should loop.
         */
        void playHeadChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Starts a choreography on the left arm.
         * @param choreography Vector of steps.
         * @param startDelay Initial delay (seconds).
         * @param loop Whether it should loop.
         */
        void playArmLChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /**
         * @brief Starts a choreography on the right arm.
         * @param choreography Vector of steps.
         * @param startDelay Initial delay (seconds).
         * @param loop Whether it should loop.
         */
        void playArmRChoreography(
            const std::vector<ChoreoStep>& choreography,
            float startDelay = 0.0f,
            bool loop = true
        );

        /** @brief Stops every active animation. */
        void stopAllAnimations();

        /**
         * @brief Computes the smooth interpolation (smoothstep).
         * @param start Initial angle.
         * @param target Destination angle.
         * @param progress Linear progress from 0.0 to 1.0.
         * @return Interpolated angle on the smooth curve.
         */
        static float easeSmoothStep(float start, float target, float progress);

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
         * @brief Processes the physics of a specific axis.
         * @return True if an initiation token was consumed.
         */
        bool updateAxisPhysics(
            MotorAxis& axis,
            float deltaTime,
            bool initiationTokenUsed
        );
};
