#pragma once

#include "Choreography.h"
#include "Common.h"
#include "FaceEmotions.hpp"
#include <Arduino.h>
#include <vector>

class DisplayOrchestrator;
class MotionController;
class AudioController;

#include "AudioTypes.h"

/**
 * @struct ChoreoAction
 * @brief Groups a sequence of steps and an initial delay for one motor.
 *
 * Used by the behavior engine to schedule specific movements on individual
 * axes while a vignette plays.
 */
struct ChoreoAction
{
        std::vector<ChoreoStep> steps; ///< Choreography steps.
        float delay = 0.0f;            ///< Initial delay in seconds.
        bool loop = false; ///< Whether the choreography should loop.

        /**
         * @brief Constructor to simplify creating actions with a delay.
         * @param s Vector of steps (frames) for the choreography.
         * @param d Wait time in seconds before starting the movement.
         * @param l Whether the movement should repeat continuously.
         */
        ChoreoAction(
            std::vector<ChoreoStep> s = {},
            float d = 0.0f,
            bool l = false
        )
            : steps(s), delay(d), loop(l)
        {
        }
};

/**
 * @struct BehaviorVignette
 * @brief Represents a "sketch" or atomic micro-behavior of the robot.
 *
 * Defines a complete expression state (eyes, gaze, motors, and sound) that
 * the robot holds for a set period of time.
 */
struct BehaviorVignette
{
        eEmotions mood;        ///< Mood/facial expression to apply.
        float lookX;           ///< Horizontal look direction (-1.0 left, 1.0
                               ///< right).
        float lookY;           ///< Vertical look direction (-1.0 down, 1.0 up).
        ChoreoAction head;     ///< Head choreography.
        ChoreoAction armL;     ///< Left arm choreography.
        ChoreoAction armR;     ///< Right arm choreography.
        AudioTrack audioTrack; ///< Optional audio to trigger (id 0 = none).
        unsigned long durationMs; ///< Display time for this vignette (ms).

        /**
         * @brief Full constructor to orchestrate multiple axes and audio.
         * @param m Visual emotion/mood.
         * @param lx Look X coordinate.
         * @param ly Look Y coordinate.
         * @param h Head movement configuration.
         * @param al Left arm movement configuration.
         * @param ar Right arm movement configuration.
         * @param d Total scene duration in milliseconds.
         * @param audio Reference to the audio track to play.
         */
        BehaviorVignette(
            eEmotions m,
            float lx,
            float ly,
            ChoreoAction h = ChoreoAction(),
            ChoreoAction al = ChoreoAction(),
            ChoreoAction ar = ChoreoAction(),
            unsigned long d = 1000,
            AudioTrack audio = {0, 0}
        )
            : mood(m), lookX(lx), lookY(ly), head(h), armL(al), armR(ar),
              audioTrack(audio), durationMs(d)
        {
        }
};

/**
 * @class BehaviorEngine
 * @brief Orchestration engine for expressive and motor behaviors.
 *
 * This class manages the execution of "vignettes" (BehaviorVignette),
 * which are synchronized combinations of facial expressions, arm/head
 * movements, and sound effects. The engine runs non-blocking, alternating
 * between periods of action and rest (organic silence).
 */
class BehaviorEngine
{
    public:
        /**
         * @brief Default constructor.
         */
        BehaviorEngine();

        /**
         * @brief Sets the available behavior set and the cadence
         * parameters.
         *
         * @param pool Vector of vignettes the robot can run.
         * @param minPauseMs Minimum rest time between actions (ms).
         * @param maxPauseMs Maximum rest time between actions (ms).
         * @param loop If true, restarts the list once it reaches the end.
         * @param resetOnRest If true, resets the pose and expression during
         * rest.
         */
        void setPool(
            const std::vector<BehaviorVignette>& pool,
            unsigned long minPauseMs = 1000,
            unsigned long maxPauseMs = 3000,
            bool loop = true,
            bool resetOnRest = false
        );

        /**
         * @brief Updates the engine's logic, managing timers and
         * triggering actions.
         *
         * Must be called continuously in the main loop or a dedicated
         * task.
         *
         * @param display Reference to the visual orchestrator.
         * @param motion Reference to the motion controller.
         * @param audio Reference to the audio controller, to trigger
         * vignette sounds.
         */
        void update(
            DisplayOrchestrator& display,
            MotionController& motion,
            AudioController& audio
        );

        /**
         * @brief Stops any ongoing action and clears the pool.
         */
        void stop();

        /**
         * @brief Pauses or resumes the engine without clearing its state.
         * @param paused True to pause, false to resume.
         */
        void setPaused(bool paused) { _isPaused = paused; }

        /**
         * @brief Checks whether the robot is currently running an active
         * action.
         * @return True if in action, false if at rest.
         */
        bool isInAction() const { return _isInAction; }

    private:
        std::vector<BehaviorVignette> _pool; ///< List of active behaviors
        int _currentIndex = -1;              ///< Current vignette index
        unsigned long _nextEventTime = 0;    ///< Timestamp for the next event
        bool _isPaused = false;              ///< Engine's pause state
        bool _isInAction = false;            ///< Whether an action is running
        bool _loop = true;                   ///< Whether to loop the pool
        bool _resetOnRest = false; ///< Whether to return to neutral during rest

        unsigned long _minPause = 1000; ///< Configured minimum pause
        unsigned long _maxPause = 3000; ///< Configured maximum pause

        /**
         * @brief Selects and triggers the next action from the pool.
         * @param display Reference to the visual orchestrator.
         * @param motion Reference to the motion controller.
         * @param audio Reference to the audio controller.
         */
        void pickNextAction(
            DisplayOrchestrator& display,
            MotionController& motion,
            AudioController& audio
        );

        /**
         * @brief Ends the current action and enters the organic rest
         * state.
         * @param display Reference to the visual orchestrator.
         * @param motion Reference to the motion controller.
         */
        void enterRest(DisplayOrchestrator& display, MotionController& motion);
};
