#pragma once
#include "State.h"

/**
 * @class SuccessState
 * @brief Celebration state upon successfully completing the wash ritual.
 *
 * This state is activated after succeeding at the drying stage. The goal
 * is to reward the child and positively reinforce the hygiene habit.
 *
 * Visual feedback: extreme happiness expression (Glee) and confetti
 * particles. Physical feedback: the robot waves its arms as a sign of
 * victory.
 */
class SuccessState : public State
{
    public:
        /**
         * @brief Activates the confetti, the victory audio, and the
         * festive mood.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Waits for the celebration to end and transitions to
         * shutdown.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Ends the success animations.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignores new reads during the final celebration.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::SUCCESS enum.
         * @return RobotState The success state.
         */
        RobotState getStateEnum() const override { return RobotState::SUCCESS; }

        /**
         * @brief Returns the celebration's display duration.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SUCCESS_DISPLAY_MS;
        }
};
