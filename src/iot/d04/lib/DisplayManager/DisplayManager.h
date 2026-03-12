#pragma once

#include "EyesManager.h"
#include "ParticleManager.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

struct DisplayConfig
{
        uint8_t screenWidth = 128;
        uint8_t screenHeight = 64;
        uint8_t oledAddress = 0x3C;
        uint32_t frameInterval = 20;
};

class DisplayManager
{
    public:
        DisplayManager(const DisplayConfig& config = DisplayConfig());
        void init();
        void update();
        void setEyeMood(EyeMood mood);
        void setEyeIdleMode(bool active);
        void blinkEyes();
        void lookAt(EyePosition position);
        void playConfused();
        void playHappy();
        void setSweat(bool active);
        void setEyesVisible(bool visible);
        void setParticleEffect(EffectType type);
        void setParticleSpawnChance(int chance);
        void setMaxParticles(int max);
        void drawBitmapCentered(
            const uint8_t* bitmap,
            uint8_t width,
            uint8_t height
        );
        void clear();
        void render();
        void prepareForSleep();
        void wakeUp();

    private:
        const DisplayConfig _config;
        Adafruit_SSD1306 _display;
        EyesManager _eyes;
        ParticleManager _animations;
        unsigned long _lastUpdate;
};
