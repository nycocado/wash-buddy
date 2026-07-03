#pragma once
#include "State.h"

/**
 * @class DryState
 * @brief Final ritual stage: drying the hands.
 *
 * This state is activated when the child brings the towel tag close. The
 * robot simulates the completion of the cleaning process through wind
 * effects and satisfaction behaviors.
 *
 * Visual feedback: wind particle effect.
 * Physical feedback: the robot performs head movements simulating that it
 * is following the towel use.
 */
class DryState : public State
{
    public:
        /**
         * @brief Starts the wind effect and the drying sound.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Manages the drying time and advances to the final
         * celebration.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Ends the wind effects and drying animations.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates whether the towel was used correctly or the
         * stage must repeat.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::DRY enum.
         * @return RobotState The drying state.
         */
        RobotState getStateEnum() const override { return RobotState::DRY; }

        /**
         * @brief Returns the pedagogical time configured for drying.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::DRY_TIMEOUT;
        }
};
