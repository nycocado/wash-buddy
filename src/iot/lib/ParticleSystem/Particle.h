#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @class Particle
 * @brief Abstract base class for every particle type.
 *
 * Defines the required interface and fundamental attributes for visual
 * effects.
 */
class Particle
{
    public:
        /** @brief Virtual destructor for cleaning up derived classes. */
        virtual ~Particle() {}

        /**
         * @brief Updates the particle's physics.
         * @param deltaTime Time elapsed (seconds).
         * @param screenWidth Screen width.
         * @param screenHeight Screen height.
         * @return true if the particle is still alive, false if it should
         * be destroyed.
         */
        virtual bool
        update(float deltaTime, int screenWidth, int screenHeight) = 0;

        /**
         * @brief Renders the particle on the display.
         * @param display Reference to the U8G2 library.
         */
        virtual void draw(U8G2& display) = 0;

    protected:
        float x;     ///< Current horizontal position.
        float y;     ///< Current vertical position.
        float speed; ///< Base speed (pixels/s).
};
