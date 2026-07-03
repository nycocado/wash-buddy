#pragma once
#include "State.h"

/**
 * @class IdleState
 * @brief Active waiting state, where the robot waits for the ritual to
 * begin.
 *
 * In this state, the robot shows "life" through random head movements,
 * waves, and facial expressions. The goal is to catch the child's
 * attention and signal that it's ready to start. The ritual begins once
 * the faucet tag is detected.
 */
class IdleState : public State
{
    public:
        /**
         * @brief Sets up the behavior pool and activates the robot's
         * organic life.
         * @param controller Pointer to the central controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Manages the random animation timers and visual reminders.
         * @param controller Pointer to the central controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Clears the behavior pool when leaving the rest state.
         * @param controller Pointer to the central controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Waits for the faucet tag to start the pedagogical journey.
         * @param controller Pointer to the central controller.
         * @param uid Unique identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Returns RobotState::IDLE. */
        RobotState getStateEnum() const override { return RobotState::IDLE; }

    private:
        unsigned long _lastReminderTime =
            0; ///< Records when the last instruction was shown
};
