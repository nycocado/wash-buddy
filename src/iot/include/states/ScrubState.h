#pragma once
#include "State.h"

/**
 * @class ScrubState
 * @brief Active hand-scrubbing stage.
 *
 * This is the central stage of the ritual, activated after applying soap.
 * The robot encourages the child to scrub their hands for the time
 * recommended by health authorities.
 *
 * Visual feedback: bubbles kept on screen.
 * Physical feedback: rhythmic scrubbing choreography with both arms.
 */
class ScrubState : public State
{
    public:
        /**
         * @brief Starts the scrubbing choreography and the rhythmic sound.
         * @param controller Pointer to the central game controller.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitors the mandatory hygiene time and transitions to
         * waiting.
         * @param controller Pointer to the central game controller.
         */
        void update(GameController* controller) override;

        /**
         * @brief Stops the scrubbing animations.
         * @param controller Pointer to the central game controller.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Validates whether the child keeps scrubbing or advances
         * to rinsing.
         * @param controller Pointer to the game controller.
         * @param uid Identifier of the detected tag.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Returns the RobotState::SCRUB enum.
         * @return RobotState The scrubbing state.
         */
        RobotState getStateEnum() const override { return RobotState::SCRUB; }

        /**
         * @brief Returns the pedagogical (WHO) time for effective
         * scrubbing.
         * @return unsigned long Time in milliseconds.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SCRUB_TIMEOUT;
        }
};
