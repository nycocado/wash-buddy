#pragma once
#include "State.h"

/**
 * @class WaitingState
 * @brief Active waiting state for interactions during the ritual.
 *
 * This state is triggered when a ritual stage ends and the robot waits
 * for the child to bring the next RFID tag close (e.g. after wetting,
 * waits for the soap). If the wait is too long (timeout), the robot
 * assumes the ritual was abandoned and shuts itself down.
 *
 * The robot shows growing impatience through "worried" and then "sad"
 * expressions, encouraging the child to return to the activity.
 */
class WaitingState : public State
{
    public:
        /**
         * @brief Starts the waiting state and shows the next stage's
         * instruction.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Manages the mood evolution and the inactivity timeout.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Ends the waiting behaviors on exit.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates whether the tag that was read matches the next
         * step or a repeat of the current stage.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::WAITING enum.
         * @return RobotState The waiting state.
         */
        RobotState getStateEnum() const override { return RobotState::WAITING; }

    private:
        bool _isWorriedPhase =
            false; ///< Whether the robot started looking for the user.
        bool _isSadPhase =
            false; ///< Whether the robot entered the disappointment phase.

        const uint8_t* _nextIcon =
            nullptr; ///< Icon for the next pedagogical stage.
        unsigned long _lastReminderTime =
            0; ///< Timestamp of the last visual reminder.
        bool _waitingAudioPlayed =
            false; ///< Whether the waiting-start audio already played.
        bool _sadAudioPlayed = false; ///< Whether the sad audio already played.
};
