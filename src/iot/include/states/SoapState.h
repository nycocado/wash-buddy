#pragma once
#include "State.h"

/**
 * @class SoapState
 * @brief Soap application stage.
 *
 * This state is activated when the child brings the soap tag close.
 * Indicates the robot recognized the start of the chemical cleaning step.
 *
 * Visual feedback: focused expression and bubble particle effect.
 * Physical feedback: the motors perform alternating movements simulating
 * receiving soap in the hands.
 */
class SoapState : public State
{
    public:
        /**
         * @brief Activates the bubbles effect and the soap-application
         * sound.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitors the application time and transitions to waiting
         * once done.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Clears the soap animations and effects.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates whether soap is still being applied or the
         * ritual advances to scrubbing.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::SOAP enum.
         * @return RobotState The soap-application state.
         */
        RobotState getStateEnum() const override { return RobotState::SOAP; }

        /**
         * @brief Returns the pedagogical time configured for applying soap.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SOAP_TIMEOUT;
        }
};
