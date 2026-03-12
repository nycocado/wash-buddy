#pragma once

#include "Particle.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

enum class EffectType
{
    NONE,
    BUBBLES,
    RAIN_LIGHT,
    RAIN_HEAVY,
    CONFETTI,
    WIND
};

struct BubbleSettings
{
        float minRadius = 2.0f;
        float maxRadius = 5.0f;
        float minSpeed = 20.0f;
        float maxSpeed = 50.0f;
        float oscillationSpeed = 0.002f;
        float oscillationAmp = 0.3f;
};

struct RainSettings
{
        float lightMinSpeed = 60.0f;
        float lightMaxSpeed = 100.0f;
        float heavyMinSpeed = 120.0f;
        float heavyMaxSpeed = 180.0f;
        float minLength = 3.0f;
        float maxLength = 12.0f;
};

struct SparkleSettings
{
        float minSpeed = 5.0f;
        float maxSpeed = 15.0f;
        float blinkSpeed = 10.0f;
};

struct ConfettiSettings
{
        float minSpeed = 40.0f;
        float maxSpeed = 100.0f;
        float gravity = 30.0f;
        float airResistance = 0.98f;
};

struct WindSettings
{
        float minSpeed = 150.0f;
        float maxSpeed = 250.0f;
        float minLength = 5.0f;
        float maxLength = 20.0f;
};

struct ParticleManagerConfig
{
        int activeLimit = 30;
        int spawnChance = 15;
        int screenWidth = 128;
        int screenHeight = 64;
};

class ParticleManager
{
    public:
        ParticleManager(
            Adafruit_SSD1306& display,
            const ParticleManagerConfig& config = ParticleManagerConfig()
        );
        ~ParticleManager();

        void setEffect(EffectType type);
        void update(float deltaTime);
        void draw();
        BubbleSettings& getBubbleSettings() { return _bubbleConfig; }
        RainSettings& getRainSettings() { return _rainConfig; }
        SparkleSettings& getSparkleConfig() { return _sparkleConfig; }
        ConfettiSettings& getConfettiConfig() { return _confettiConfig; }
        WindSettings& getWindSettings() { return _windConfig; }
        void setSpawnChance(int chance) { _config.spawnChance = chance; }
        void setMaxParticles(int max)
        {
            _config.activeLimit = (max > MAX_PARTICLES) ? MAX_PARTICLES : max;
        }

    private:
        Adafruit_SSD1306& _display;
        ParticleManagerConfig _config;
        static constexpr int MAX_PARTICLES = 40;
        Particle* _particles[MAX_PARTICLES];
        EffectType _currentEffect;
        BubbleSettings _bubbleConfig;
        RainSettings _rainConfig;
        SparkleSettings _sparkleConfig;
        ConfettiSettings _confettiConfig;
        WindSettings _windConfig;

        void spawnParticle(int index);
        void clearAll();
};
