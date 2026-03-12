#pragma once

#include "Particle.h"
#include "ParticleManager.h"

class BubbleParticle : public Particle
{
    public:
        BubbleParticle(float startX, float startY, const BubbleSettings& cfg);
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;
        void draw(Adafruit_SSD1306& display) override;

    private:
        float radius;
        float offset;
        bool isPopping;
        float popTimer;
        const BubbleSettings& _cfg;
};

class RainParticle : public Particle
{
    public:
        RainParticle(
            float startX,
            float startY,
            const RainSettings& cfg,
            bool isHeavy
        );
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;
        void draw(Adafruit_SSD1306& display) override;

    private:
        float length;
        bool isSplashing;
        float splashTimer;
        const RainSettings& _cfg;
};

class SparkleParticle : public Particle
{
    public:
        SparkleParticle(float startX, float startY, const SparkleSettings& cfg);
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;
        void draw(Adafruit_SSD1306& display) override;

    private:
        float blinkTimer;
        const SparkleSettings& _cfg;
};

class ConfettiParticle : public Particle
{
    public:
        ConfettiParticle(
            float startX,
            float startY,
            const ConfettiSettings& cfg
        );
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;
        void draw(Adafruit_SSD1306& display) override;

    private:
        float vx, vy;
        float size;
        float spinSpeed;
        float spinPhase;
        const ConfettiSettings& _cfg;
};

class WindParticle : public Particle
{
    public:
        WindParticle(float startX, float startY, const WindSettings& cfg);
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;
        void draw(Adafruit_SSD1306& display) override;

    private:
        float baseY;
        float length;
        float waveFrequency;
        float waveAmplitude;
        float phase;
        const WindSettings& _cfg;
};
