#pragma once

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

class Particle
{
    public:
        virtual ~Particle() {}
        virtual bool
        
        update(float deltaTime, int screenWidth, int screenHeight) = 0;
        virtual void draw(Adafruit_SSD1306& display) = 0;

    protected:
        float x, y;
        float speed;
};
