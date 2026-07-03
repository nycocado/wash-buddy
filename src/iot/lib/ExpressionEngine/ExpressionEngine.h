#pragma once

#include "Face.h"
#include <Arduino.h>

/**
 * @brief Basic settings for the face/expression behavior.
 */
struct ExpressionConfig
{
        int blinkInterval = 3500; ///< Average interval between blinks (ms)
        uint16_t eyeSize = 40;    ///< Base eye size in pixels
};

/**
 * @class ExpressionEngine
 * @brief Manages the rendering and expressive behavior of the eyes.
 *
 * Wraps the esp32-eyes library logic, providing a semantic interface to
 * control emotions, gaze direction, and automatic animations.
 */
class ExpressionEngine
{
    public:
        ExpressionEngine();
        ~ExpressionEngine();

        /**
         * @brief Initializes the eye rendering engine.
         * @param width Screen width in pixels.
         * @param height Screen height in pixels.
         * @param config Optional configuration structure.
         */
        void begin(
            uint16_t width,
            uint16_t height,
            const ExpressionConfig& config = ExpressionConfig()
        );

        /** @brief Updates the internal animation and behavior states. */
        void update();

        /** @brief Draws the eyes into the current U8g2 buffer. */
        void draw();

        /**
         * @brief Changes the current emotion/expression.
         * @param mood eEmotions enum (e.g. Focused, Happy, Sad).
         */
        void setMood(eEmotions mood);

        /**
         * @brief Enables/disables automatic idle behaviors.
         * @param active If true, the robot blinks and looks around on its
         * own.
         */
        void setIdleMode(bool active);

        /** @brief Runs an immediate manual blink cycle. */
        void blink();

        /**
         * @brief Points the gaze toward normalized coordinates.
         * @param x Horizontal position (-1.0 to 1.0).
         * @param y Vertical position (-1.0 to 1.0).
         */
        void lookAt(float x, float y);

        /** @brief Plays the confused animation (skeptical look). */
        void playConfused();

        /** @brief Plays the happy animation (bright-eyed look). */
        void playHappy();

    private:
        Face* _face;     ///< Pointer to the face's physical implementation
        bool _isVisible; ///< Rendering control flag
};
