#pragma once

#include "Common.h"
#include "ExpressionEngine.h"
#include "ParticleSystem.h"
#include <Arduino.h>

/**
 * @brief Basic hardware and timing settings for the display.
 */
struct DisplayConfig
{
        uint8_t screenWidth = 128;    ///< Screen width in pixels
        uint8_t screenHeight = 64;    ///< Screen height in pixels
        uint32_t frameInterval = 16;  ///< Interval between frames (ms) ~60fps
        float transitionSpeed = 5.0f; ///< Curtain transition speed
        uint8_t iconDefaultSize = 64; ///< Default (square) icon size
};

/**
 * @class DisplayOrchestrator
 * @brief Central orchestrator for all visual output on the OLED.
 *
 * This class is responsible for synchronizing the drawing of the
 * expressions (ExpressionEngine) and the particle systems (ParticleSystem)
 * into a single U8g2 buffer, ensuring the screen updates smoothly and
 * without flickering.
 */
class DisplayOrchestrator
{
    public:
        /**
         * @brief Orchestrator constructor.
         * @param config Structure with the screen settings.
         */
        DisplayOrchestrator(const DisplayConfig& config = DisplayConfig());

        /**
         * @brief Initializes the display hardware and its sub-systems.
         * Configures the I2C pins and prepares the internal animation
         * engines.
         */
        void init();

        /**
         * @brief Updates the animation logic and renders the next frame.
         * @param deltaTime Time elapsed since the last frame (seconds).
         */
        void update(float deltaTime);

        /**
         * @brief Sets the current mood/expression.
         * @param mood One of the eEmotions enum values (e.g. Happy, Angry).
         */
        void setEyeMood(eEmotions mood);

        /**
         * @brief Points the gaze toward a specific coordinate.
         * @param x Horizontal position (-1.0 left, 1.0 right).
         * @param y Vertical position (-1.0 down, 1.0 up).
         */
        void lookAt(float x, float y);

        /**
         * @brief Plays a short confusion/doubt animation.
         */
        void playConfused();

        /**
         * @brief Plays a short happiness/celebration animation.
         */
        void playHappy();

        /**
         * @brief Sets the active atmospheric particle effect.
         * @param type Effect type (e.g. BUBBLES, RAIN, DRIP).
         */
        void setParticleEffect(EffectType type);

        /**
         * @brief Shows an instruction icon for a set amount of time.
         * Once the time expires, the display goes back to showing the
         * eyes.
         * @param icon PROGMEM pointer to the XBM.
         * @param durationMs Time in milliseconds (0 for fixed).
         */
        void
        showInstruction(const uint8_t* icon, unsigned long durationMs = 3000);

        /**
         * @brief Immediately removes any instruction icon and forces a
         * return to the eyes.
         */
        void clearInstruction();

        /**
         * @brief Checks whether an instruction icon or a transition is
         * active.
         * @return true if an instruction icon is being shown or a curtain
         * transition is in progress, false otherwise.
         */
        bool isInstructionActive() const
        {
            return (_currentInstructionIcon != nullptr) ||
                   (_pendingIcon != nullptr) ||
                   (_transitionState != TransitionState::IDLE);
        }

        /**
         * @brief Puts the display controller into power-save mode.
         */
        void prepareForSleep();

        /**
         * @brief Shows a debug text in the top corner of the screen.
         * @param text The text to display. Leave empty ("") to hide it.
         */
        void setDebugText(const String& text);

    private:
        const DisplayConfig _config;
        ExpressionEngine _eyes;
        ParticleSystem _animations;
        unsigned long _lastUpdate;
        String _debugText;

        const uint8_t* _currentInstructionIcon = nullptr;
        const uint8_t* _pendingIcon = nullptr; // Next icon to be shown
        uint8_t _instructionWidth = 64;
        uint8_t _instructionHeight = 64;
        unsigned long _instructionStartTime = 0;
        unsigned long _instructionEndTime = 0;

        // Transition control
        enum class TransitionState
        {
            IDLE,
            CLOSING,
            OPENING
        };
        TransitionState _transitionState = TransitionState::IDLE;
        float _transitionProgress = 0.0f;
};
