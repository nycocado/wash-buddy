#pragma once

#include "BubbleParticle.h"
#include "ConfettiParticle.h"
#include "Particle.h"
#include "RainParticle.h"
#include "WindParticle.h"
#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @brief Available particle visual effect types.
 */
enum class EffectType
{
    NONE,       ///< No effect
    BUBBLES,    ///< Rising bubbles
    RAIN_LIGHT, ///< Light rain
    RAIN_HEAVY, ///< Heavy rain
    CONFETTI,   ///< Confetti burst
    WIND        ///< Horizontal streaks
};

/**
 * @brief Global settings for the particle system.
 */
struct ParticleSystemConfig
{
        int activeLimit = 30;  ///< Maximum number of simultaneous particles
        int spawnChance = 15;  ///< Spawn probability (0-100) per frame
        int screenWidth = 128; ///< Width of the spawn area
        int screenHeight = 64; ///< Height of the spawn area
};

/**
 * @class ParticleSystem
 * @brief Particle-based special effects engine.
 *
 * Manages the lifecycle of the various particle types that decorate the
 * display.
 */
class ParticleSystem
{
    public:
        /**
         * @brief System constructor.
         * @param display Reference to U8g2.
         * @param config Global limit settings.
         */
        ParticleSystem(
            U8G2& display,
            const ParticleSystemConfig& config = ParticleSystemConfig()
        );

        /** @brief Destructor for memory cleanup. */
        ~ParticleSystem();

        /**
         * @brief Changes the active atmospheric effect.
         * @param type New effect type.
         */
        void setEffect(EffectType type);

        /**
         * @brief Updates the physics and lifecycle of every particle.
         * @param deltaTime Time elapsed (seconds).
         */
        void update(float deltaTime);

        /** @brief Renders every active particle. */
        void draw();

        /** @brief Sets the spawn chance for new particles. */
        void setSpawnChance(int chance) { _config.spawnChance = chance; }

        /** @brief Sets the maximum number of live particles. */
        void setMaxParticles(int max)
        {
            _config.activeLimit = (max > MAX_PARTICLES) ? MAX_PARTICLES : max;
        }

    private:
        U8G2& _display;
        ParticleSystemConfig _config;
        static constexpr int MAX_PARTICLES = 40;
        Particle* _particles[MAX_PARTICLES];
        EffectType _currentEffect;

        BubbleSettings _bubbleConfig;
        RainSettings _rainConfig;
        ConfettiSettings _confettiConfig;
        WindSettings _windConfig;

        /** @brief Instantiates a new particle in the given slot. */
        void spawnParticle(int index);

        /** @brief Removes and deletes every particle. */
        void clearAll();
};
