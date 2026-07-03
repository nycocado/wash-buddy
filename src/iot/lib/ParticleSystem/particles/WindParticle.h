#pragma once

#include "Particle.h"

/**
 * @struct WindSettings
 * @brief Physical settings for the wind effect.
 */
struct WindSettings
{
        float minSpeed = 150.0f; ///< Minimum horizontal speed (px/s).
        float maxSpeed = 250.0f; ///< Maximum horizontal speed (px/s).
        float minLength = 5.0f;  ///< Minimum gust length.
        float maxLength = 20.0f; ///< Maximum gust length.
};

/**
 * @class WindParticle
 * @brief Wind particle with wavy motion.
 *
 * Simulates air gusts through lines that oscillate vertically.
 */
class WindParticle : public Particle
{
    public:
        /**
         * @brief Gust constructor.
         * @param startX Initial horizontal position.
         * @param startY Base vertical position.
         * @param cfg Wind settings.
         */
        WindParticle(float startX, float startY, const WindSettings& cfg);

        /**
         * @brief Updates the horizontal translation.
         * @param deltaTime Time elapsed (seconds).
         * @param screenWidth Screen width.
         * @param screenHeight Screen height.
         * @return True if the gust is still visible.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renders the wavy line point by point. */
        void draw(U8G2& display) override;

    private:
        float baseY;
        float length;
        float waveFrequency;
        float waveAmplitude;
        float phase;
        const WindSettings& _cfg;
};
