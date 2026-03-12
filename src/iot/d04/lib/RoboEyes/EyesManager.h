#pragma once

#include "FluxGarage_RoboEyes.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

struct EyesConfig
{
        int blinkInterval = 3;
        int blinkVariation = 2;
        int idleInterval = 2;
        int idleVariation = 2;
};

class EyesManager
{
    public:
        EyesManager(Adafruit_SSD1306& display);
        ~EyesManager();

        void begin(const EyesConfig& config = EyesConfig());
        void update();
        void setVisibility(bool visible);
        bool isVisible() const { return _isVisible; }
        void setMood(EyeMood mood);
        void setIdleMode(bool active);
        void blink();
        void lookAt(EyePosition position);
        void playConfused();
        void playHappy();
        void setSweat(bool active);

    private:
        Adafruit_SSD1306& _display;
        void* _eyesInstance;
        bool _isVisible;
};
