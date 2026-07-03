#pragma once

#include <Arduino.h>
#include <vector>

/**
 * @struct ChoreoStep
 * @brief Represents a single "step" within a movement sequence.
 */
struct ChoreoStep
{
        int targetAngle; ///< Destination angle for the servo (0-180).
        float duration;  ///< Time this step holds/runs for (in
                         ///< seconds).
};

/**
 * @class ChoreoSequence
 * @brief Playback engine for movement sequences (choreographies) on a
 * single axis.
 *
 * This class manages the timing and transition between multiple
 * ChoreoSteps, allowing complex, repeating (loop) or one-shot movements.
 */
class ChoreoSequence
{
    public:
        /**
         * @brief Constructor for the sequence manager.
         */
        ChoreoSequence();

        /**
         * @brief Sets the frames/steps that make up the choreography.
         * @param steps Vector containing the sequence's steps.
         */
        void setFrames(const std::vector<ChoreoStep>& steps);

        /**
         * @brief Starts playing the choreography.
         * @param startDelay Optional delay (in seconds) before the first
         * step starts.
         * @param loop If true, the sequence restarts automatically once it
         * reaches the end.
         */
        void play(float startDelay = 0.0f, bool loop = true);

        /**
         * @brief Stops playback immediately.
         */
        void stop();

        /**
         * @brief Checks whether the choreography is actively playing.
         * @return True if running, false if stopped or in the initial
         * delay.
         */
        bool isActive() const;

        /**
         * @brief Updates the internal timer and determines the current
         * step. Must be called on every main loop frame.
         * @param deltaTime Time elapsed since the last frame (seconds).
         * @return Pointer to the current ChoreoStep if the command
         * changed, or nullptr.
         */
        const ChoreoStep* update(float deltaTime);

    private:
        std::vector<ChoreoStep> _steps;
        size_t _currentStepIndex;
        float _timer;
        bool _isActive;
        bool _isLooping;
        bool _isFirstFrame;
};
