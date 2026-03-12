#include "ParticleTypes.h"
#include <math.h>

BubbleParticle::BubbleParticle(
    float startX,
    float startY,
    const BubbleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;
    radius = _cfg.minRadius + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxRadius - _cfg.minRadius);
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);
    offset = (float)random(0, 360);
    isPopping = false;
    popTimer = 0.0f;
}

bool BubbleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isPopping)
    {
        y -= speed * deltaTime;
        x += sin(millis() * _cfg.oscillationSpeed + offset) *
             _cfg.oscillationAmp;

        if (y < -radius || (y < (screenHeight / 2) && random(0, 1000) < 5))
        {
            isPopping = true;
        }
        return true;
    }
    else
    {
        popTimer += deltaTime;
        return (popTimer < 0.1f);
    }
}

void BubbleParticle::draw(Adafruit_SSD1306& display)
{
    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;
    int16_t ir = (int16_t)radius;

    if (!isPopping)
    {
        display.drawCircle(ix, iy, ir, SSD1306_WHITE);

        if (ir == 2)
        {
            display.drawPixel(ix - 1, iy - 1, SSD1306_WHITE);
        }
        else if (ir == 3)
        {
            display.drawPixel(ix - 1, iy - 2, SSD1306_WHITE);
        }
        else if (ir == 4)
        {
            display.drawPixel(ix - 2, iy - 2, SSD1306_WHITE);
            display.drawPixel(ix - 1, iy - 3, SSD1306_WHITE);
        }
        else if (ir >= 5)
        {
            display.drawPixel(ix - 2, iy - 3, SSD1306_WHITE);
            display.drawPixel(ix - 3, iy - 2, SSD1306_WHITE);
        }
    }
    else
    {
        int spread = (int)(popTimer * 50.0f) + 1;
        display.drawPixel(ix - spread, iy - spread, SSD1306_WHITE);
        display.drawPixel(ix + spread, iy - spread, SSD1306_WHITE);
        display.drawPixel(ix - spread, iy + spread, SSD1306_WHITE);
        display.drawPixel(ix + spread, iy + spread, SSD1306_WHITE);
    }
}

RainParticle::RainParticle(
    float startX,
    float startY,
    const RainSettings& cfg,
    bool isHeavy
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;
    float minS = isHeavy ? _cfg.heavyMinSpeed : _cfg.lightMinSpeed;
    float maxS = isHeavy ? _cfg.heavyMaxSpeed : _cfg.lightMaxSpeed;
    speed = minS + (float)(random(0, 100) / 100.0f) * (maxS - minS);
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);
    isSplashing = false;
    splashTimer = 0.0f;
}

bool RainParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    if (!isSplashing)
    {
        y += speed * deltaTime;
        if (y >= screenHeight)
        {
            isSplashing = true;
            y = screenHeight;
        }
        return true;
    }
    else
    {
        splashTimer += deltaTime;
        return (splashTimer < 0.15f);
    }
}

void RainParticle::draw(Adafruit_SSD1306& display)
{
    if (!isSplashing)
    {
        display.drawFastVLine(
            (int16_t)x, (int16_t)(y - length), (int16_t)length, SSD1306_WHITE
        );
    }
    else
    {
        int spread = (int)(splashTimer * 30.0f);
        int floorY = display.height() - 1;
        display.drawPixel((int16_t)(x - spread - 1), floorY, SSD1306_WHITE);
        display.drawPixel((int16_t)(x + spread + 1), floorY, SSD1306_WHITE);
        if (splashTimer < 0.08f)
        {
            display.drawPixel((int16_t)x, floorY - spread, SSD1306_WHITE);
        }
    }
}

SparkleParticle::SparkleParticle(
    float startX,
    float startY,
    const SparkleSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);
    blinkTimer = (float)random(0, 314) / 100.0f;
}

bool SparkleParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    y -= speed * deltaTime;
    blinkTimer += _cfg.blinkSpeed * deltaTime;
    return (y > -5);
}

void SparkleParticle::draw(Adafruit_SSD1306& display)
{
    float intensity = (sin(blinkTimer) + 1.0f) * 1.5f;

    int16_t ix = (int16_t)x;
    int16_t iy = (int16_t)y;

    if (intensity > 0.5f)
    {
        display.drawPixel(ix, iy, SSD1306_WHITE);
    }
    if (intensity > 1.5f)
    {
        display.drawPixel(ix + 1, iy, SSD1306_WHITE);
        display.drawPixel(ix - 1, iy, SSD1306_WHITE);
        display.drawPixel(ix, iy + 1, SSD1306_WHITE);
        display.drawPixel(ix, iy - 1, SSD1306_WHITE);
    }
    if (intensity > 2.5f)
    {
        display.drawPixel(ix + 2, iy, SSD1306_WHITE);
        display.drawPixel(ix - 2, iy, SSD1306_WHITE);
        display.drawPixel(ix, iy + 2, SSD1306_WHITE);
        display.drawPixel(ix, iy - 2, SSD1306_WHITE);
    }
}

ConfettiParticle::ConfettiParticle(
    float startX,
    float startY,
    const ConfettiSettings& cfg
)
    : _cfg(cfg)
{
    x = startX;
    y = startY;
    float angle = (float)random(0, 360) * (M_PI / 180.0f);
    float mag = _cfg.minSpeed + (float)(random(0, 100) / 100.0f) *
                                    (_cfg.maxSpeed - _cfg.minSpeed);
    vx = cos(angle) * mag;
    vy = sin(angle) * mag;
    size = (float)random(2, 5);
    spinSpeed = (float)random(5, 20);
    spinPhase = (float)random(0, 314) / 100.0f;
}

bool ConfettiParticle::update(
    float deltaTime,
    int screenWidth,
    int screenHeight
)
{
    x += vx * deltaTime;
    y += vy * deltaTime;
    vy += _cfg.gravity * deltaTime;
    vx *= _cfg.airResistance;
    vy *= _cfg.airResistance;

    spinPhase += spinSpeed * deltaTime;

    return (y < screenHeight && x >= -5 && x <= screenWidth + 5);
}

void ConfettiParticle::draw(Adafruit_SSD1306& display)
{
    float currentWidth = abs(cos(spinPhase)) * size;

    int drawW = max(1, (int)currentWidth);
    int drawH = (int)size;

    display.fillRect(
        (int16_t)(x - drawW / 2.0f),
        (int16_t)(y - drawH / 2.0f),
        drawW,
        drawH,
        SSD1306_WHITE
    );
}

WindParticle::WindParticle(float startX, float startY, const WindSettings& cfg)
    : _cfg(cfg)
{
    x = startX;
    baseY = startY;
    y = startY;
    speed = _cfg.minSpeed +
            (float)(random(0, 100) / 100.0f) * (_cfg.maxSpeed - _cfg.minSpeed);
    length = _cfg.minLength + (float)(random(0, 100) / 100.0f) *
                                  (_cfg.maxLength - _cfg.minLength);

    waveFrequency = (float)random(5, 15) / 100.0f;
    waveAmplitude = (float)random(2, 6);
    phase = (float)random(0, 314) / 100.0f;
}

bool WindParticle::update(float deltaTime, int screenWidth, int screenHeight)
{
    x += speed * deltaTime;
    return (x - length < screenWidth);
}

void WindParticle::draw(Adafruit_SSD1306& display)
{
    int startX = (int)x;
    int endX = startX - (int)length;

    for (int currX = startX; currX > endX; currX--)
    {
        if (currX >= 0 && currX < display.width())
        {
            int currY = (int
            )(baseY + sin((float)currX * waveFrequency + phase) * waveAmplitude
            );
            display.drawPixel(currX, currY, SSD1306_WHITE);
        }
    }
}
