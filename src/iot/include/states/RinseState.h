#pragma once
#include "State.h"

/**
 * @class RinseState
 * @brief Hand-rinsing stage.
 *
 * Activated when the child brings the faucet tag close after the
 * scrubbing stage. Simulates removing the soap under running water.
 *
 * Visual feedback: heavy rain effect.
 * Physical feedback: the robot performs "dipping" arm movements,
 * simulating the position of the hands under the water.
 */
class RinseState : public State
{
    public:
        /**
         * @brief Activates the heavy rain effect and the rinsing sound.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitors the rinsing time and transitions to waiting once
         * done.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Clears the rinsing animations and effects.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates faucet (repeat) or towel (advance) tags.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::RINSE enum.
         * @return RobotState The rinsing state.
         */
        RobotState getStateEnum() const override { return RobotState::RINSE; }

        /**
         * @brief Returns the pedagogical time configured for rinsing.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::RINSE_TIMEOUT;
        }
};
