#pragma once

#include "Constants.h"
#include "RobotState.h"
#include <Arduino.h>

class GameController;

/**
 * @class State
 * @brief Base interface for the robot's state implementations.
 *
 * Defines the contract that every concrete state must follow, using the
 * State design pattern to decouple the behavior logic.
 */
class State
{
    public:
        /** @brief Virtual destructor for cleaning up subclasses. */
        virtual ~State() = default;

        /**
         * @brief Called once when entering this state.
         * @param controller Pointer to the FSM's main context.
         */
        virtual void enter(GameController* controller) = 0;

        /**
         * @brief Called repeatedly on every main loop cycle.
         * @param controller Pointer to the FSM's main context.
         */
        virtual void update(GameController* controller) = 0;

        /**
         * @brief Called once when leaving this state.
         * @param controller Pointer to the FSM's main context.
         */
        virtual void exit(GameController* controller) = 0;

        /**
         * @brief Processes the detection of an RFID tag.
         * @param controller Pointer to the FSM's main context.
         * @param uid Unique identifier of the tag read by the sensor.
         */
        virtual void
        handleRFID(GameController* controller, const String& uid) = 0;

        /**
         * @brief Returns this state's unique identifier.
         * @return The enum value corresponding to the state.
         */
        virtual RobotState getStateEnum() const = 0;

        /**
         * @brief Returns the allowed duration for this state.
         * @return Timeout in milliseconds.
         */
        virtual unsigned long getTimeout() const
        {
            return GameConfig::STATE_TIMEOUT_DEFAULT;
        }

    protected:
        /**
         * @brief Helper function to standardize RFID tag validation.
         * @param controller FSM context.
         * @param uid UID of the tag that was read.
         * @param currentTag Expected UID to repeat the current state.
         * @param nextTag Expected UID to advance the ritual.
         * @param nextState Destination state when advancing.
         */
        void validateRFID(
            GameController* controller,
            const String& uid,
            const char* currentTag,
            const char* nextTag,
            RobotState nextState
        );
};
