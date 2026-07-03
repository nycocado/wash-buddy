#pragma once
#include "State.h"

/**
 * @class WetState
 * @brief First active stage of the ritual: wetting the hands.
 *
 * This state is activated when the child brings the faucet tag close
 * while the robot is in Idle. The goal is to prepare the hands for the
 * soap.
 *
 * Visual feedback: light rain effect.
 * Physical feedback: the robot performs "dipping" movements simulating
 * interaction with the water.
 */
class WetState : public State
{
    public:
        /**
         * @brief Activates the light rain effect and starts the water
         * sound.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Manages the time exposed to water and transitions to
         * waiting.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Ends the water animations and effects.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates faucet (repeat) or soap (advance) tags.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::WET enum.
         * @return RobotState The wetting-hands state.
         */
        RobotState getStateEnum() const override { return RobotState::WET; }

        /**
         * @brief Returns the pedagogical time configured for wetting the
         * hands.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::WET_TIMEOUT;
        }
};
