#pragma once
#include "State.h"

/**
 * @class ErrorState
 * @brief Attention state triggered by incorrect or out-of-sequence actions.
 *
 * This state interrupts the normal flow when the child uses an item that
 * isn't expected for the current stage of the ritual (e.g. trying to use
 * the towel right after the soap). The robot shows confusion to signal
 * that attention is needed.
 *
 * Visual feedback: confused eye animation.
 * Physical feedback: the robot shakes its head "no" and raises its arms.
 */
class ErrorState : public State
{
    public:
        /**
         * @brief Triggers the error audio and activates the confused
         * expression.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Waits for the error feedback to end and returns to the
         * appropriate state.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Clears the error state and releases the behavior.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignores new reads while the error message is active.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::ERROR enum.
         * @return RobotState The error state.
         */
        RobotState getStateEnum() const override { return RobotState::ERROR; }
};
