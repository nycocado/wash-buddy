#pragma once

#include "Particle.h"

/**
 * @struct RainSettings
 * @brief Settings for the rain effect.
 */
struct RainSettings
{
        float lightMinSpeed = 60.0f;  ///< Minimum speed (light rain).
        float lightMaxSpeed = 100.0f; ///< Maximum speed (light rain).
        float heavyMinSpeed = 120.0f; ///< Minimum speed (heavy rain).
        float heavyMaxSpeed = 180.0f; ///< Maximum speed (heavy rain).
        float minLength = 3.0f;       ///< Minimum drop length.
        float maxLength = 12.0f;      ///< Maximum drop length.
};

/**
 * @class RainParticle
 * @brief Rain particle with a splash effect.
 *
 * Simulates drops falling vertically that turn into splashes on impact.
 */
class RainParticle : public Particle
{
    public:
        /**
         * @brief Drop constructor.
         * @param startX Horizontal position.
         * @param startY Vertical position.
         * @param cfg Rain settings.
         * @param isHeavy Whether to use heavy-rain parameters.
         */
        RainParticle(
            float startX,
            float startY,
            const RainSettings& cfg,
            bool isHeavy
        );

        /**
         * @brief Updates the fall and splash timer.
         * @param deltaTime Time elapsed (seconds).
         * @param screenWidth Screen width.
         * @param screenHeight Collision point.
         * @return True if the drop or its splash is still active.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renders the falling drop or its impact effect. */
        void draw(U8G2& display) override;

    private:
        float length;
        bool isSplashing;
        float splashTimer;
        const RainSettings& _cfg;
};
