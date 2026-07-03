#pragma once
#include "State.h"

/**
 * @class BootState
 * @brief Initial boot and calibration state of the robot.
 *
 * This state is the FSM's entry point right after power-on. Its main
 * purpose is to make sure every actuator (servos) is in a safe, centered
 * position while waiting for the audio hardware to be ready.
 */
class BootState : public State
{
    public:
        /**
         * @brief Centers the servos and starts the boot audio sequence.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Waits for the hardware's electrical and logical
         * stabilization.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Finishes the boot routines.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignores any tag reads during the boot process.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::BOOT enum.
         * @return RobotState The boot state.
         */
        RobotState getStateEnum() const override { return RobotState::BOOT; }
};
