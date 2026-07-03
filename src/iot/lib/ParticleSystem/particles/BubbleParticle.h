#pragma once

#include "Particle.h"

/**
 * @struct BubbleSettings
 * @brief Physical and visual settings for the bubble effect.
 */
struct BubbleSettings
{
        float minRadius = 2.0f;          ///< Minimum radius (pixels).
        float maxRadius = 5.0f;          ///< Maximum radius (pixels).
        float minSpeed = 20.0f;          ///< Minimum rise speed (px/s).
        float maxSpeed = 50.0f;          ///< Maximum rise speed (px/s).
        float oscillationSpeed = 0.002f; ///< Oscillation speed.
        float oscillationAmp = 0.3f;     ///< Oscillation amplitude (pixels).
};

/**
 * @class BubbleParticle
 * @brief Bubble particle with sinusoidal rise.
 *
 * Simulates soap bubbles rising while oscillating horizontally and
 * popping.
 */
class BubbleParticle : public Particle
{
    public:
        /**
         * @brief Bubble constructor.
         * @param startX Initial X position.
         * @param startY Initial Y position.
         * @param cfg Behavior settings.
         */
        BubbleParticle(float startX, float startY, const BubbleSettings& cfg);

        /**
         * @brief Updates the rise and oscillation physics.
         * @param deltaTime Time elapsed (seconds).
         * @param screenWidth Screen width.
         * @param screenHeight Screen height.
         * @return True if the particle is still active.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renders the bubble or its pop animation. */
        void draw(U8G2& display) override;

    private:
        float radius;
        float offset;
        bool isPopping;
        float popTimer;
        const BubbleSettings& _cfg;
};
