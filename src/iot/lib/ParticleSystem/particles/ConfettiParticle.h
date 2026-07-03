#pragma once

#include "Particle.h"

/**
 * @struct ConfettiSettings
 * @brief Physical settings for the confetti effect.
 */
struct ConfettiSettings
{
        float minSpeed = 40.0f;      ///< Minimum initial speed (px/s).
        float maxSpeed = 100.0f;     ///< Maximum initial speed (px/s).
        float gravity = 30.0f;       ///< Applied gravity (px/s^2).
        float airResistance = 0.98f; ///< Air resistance factor.
};

/**
 * @class ConfettiParticle
 * @brief Confetti particle with projectile physics and rotation.
 *
 * Simulates colored paper pieces falling erratically with simplified 3D
 * rotation.
 */
class ConfettiParticle : public Particle
{
    public:
        /**
         * @brief Confetti constructor.
         * @param startX Launch X position.
         * @param startY Launch Y position.
         * @param cfg Physics settings.
         */
        ConfettiParticle(
            float startX,
            float startY,
            const ConfettiSettings& cfg
        );

        /**
         * @brief Updates the trajectory and visual rotation.
         * @param deltaTime Time elapsed (seconds).
         * @param screenWidth Screen width.
         * @param screenHeight Screen height.
         * @return True if the particle is still visible.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /** @brief Renders the confetti with the rotation effect. */
        void draw(U8G2& display) override;

    private:
        float vx, vy;
        float size;
        float spinSpeed;
        float spinPhase;
        const ConfettiSettings& _cfg;
};
