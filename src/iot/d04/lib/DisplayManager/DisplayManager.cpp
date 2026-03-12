#include "DisplayManager.h"

DisplayManager::DisplayManager(const DisplayConfig& config)
    : _config(config),
      _display(_config.screenWidth, _config.screenHeight, &Wire, -1),
      _eyes(_display), _animations(_display), _lastUpdate(0)
{
}

void DisplayManager::init()
{
    if (!_display.begin(SSD1306_SWITCHCAPVCC, _config.oledAddress))
    {
        Serial.println(F("[DISPLAY] SSD1306 allocation failed"));
        return;
    }

    _display.clearDisplay();
    _display.display();

    _eyes.begin();
    _eyes.setVisibility(true);

    Serial.println(F("[DISPLAY] Initialized successfully"));
}

void DisplayManager::update()
{
    unsigned long now = millis();
    if (now - _lastUpdate < _config.frameInterval)
    {
        return;
    }

    float deltaTime = (now - _lastUpdate) / 1000.0f;
    _lastUpdate = now;

    _display.clearDisplay();

    _eyes.update();
    _animations.update(deltaTime);
    _animations.draw();

    _display.display();
}

void DisplayManager::setEyeMood(EyeMood mood) { _eyes.setMood(mood); }

void DisplayManager::setEyeIdleMode(bool active) { _eyes.setIdleMode(active); }

void DisplayManager::blinkEyes() { _eyes.blink(); }

void DisplayManager::lookAt(EyePosition position) { _eyes.lookAt(position); }

void DisplayManager::playConfused() { _eyes.playConfused(); }

void DisplayManager::playHappy() { _eyes.playHappy(); }

void DisplayManager::setSweat(bool active) { _eyes.setSweat(active); }

void DisplayManager::setEyesVisible(bool visible)
{
    _eyes.setVisibility(visible);
}

void DisplayManager::setParticleEffect(EffectType type)
{
    _animations.setEffect(type);
}

void DisplayManager::setParticleSpawnChance(int chance)
{
    _animations.setSpawnChance(chance);
}

void DisplayManager::setMaxParticles(int max)
{
    _animations.setMaxParticles(max);
}

void DisplayManager::drawBitmapCentered(
    const uint8_t* bitmap,
    uint8_t width,
    uint8_t height
)
{
    int16_t x = (_config.screenWidth - width) / 2;
    int16_t y = (_config.screenHeight - height) / 2;

    _display.drawBitmap(x, y, bitmap, width, height, SSD1306_WHITE);
}

void DisplayManager::render() { _display.display(); }

void DisplayManager::clear() { _display.clearDisplay(); }

void DisplayManager::prepareForSleep()
{
    _display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void DisplayManager::wakeUp() { _display.ssd1306_command(SSD1306_DISPLAYON); }
