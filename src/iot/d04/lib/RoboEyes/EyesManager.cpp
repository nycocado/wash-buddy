#include "EyesManager.h"

inline RoboEyes<Adafruit_SSD1306>* getEyes(void* instance)
{
    return static_cast<RoboEyes<Adafruit_SSD1306>*>(instance);
}

EyesManager::EyesManager(Adafruit_SSD1306& display)
    : _display(display), _isVisible(true)
{
    _eyesInstance = new RoboEyes<Adafruit_SSD1306>(_display);
}

EyesManager::~EyesManager()
{
    if (_eyesInstance)
    {
        delete getEyes(_eyesInstance);
    }
}

void EyesManager::begin(const EyesConfig& config)
{
    auto eyes = getEyes(_eyesInstance);
    eyes->begin(128, 64, 50);
    eyes->setAutoblinker(true, config.blinkInterval, config.blinkVariation);
    eyes->setIdleMode(true, config.idleInterval, config.idleVariation);
    eyes->open();
}

void EyesManager::update()
{
    if (_isVisible && _eyesInstance)
    {
        getEyes(_eyesInstance)->update();
    }
}

void EyesManager::setVisibility(bool visible) { _isVisible = visible; }

void EyesManager::setMood(EyeMood mood)
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->setMood(mood);
}

void EyesManager::setIdleMode(bool active)
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->setIdleMode(active);
}

void EyesManager::blink()
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->blink();
}

void EyesManager::lookAt(EyePosition position)
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->setPosition(position);
}

void EyesManager::playConfused()
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->anim_confused();
}

void EyesManager::playHappy()
{
    if (_eyesInstance)
    {
        getEyes(_eyesInstance)->setMood(EyeMood::HAPPY);
        getEyes(_eyesInstance)->anim_laugh();
    }
}

void EyesManager::setSweat(bool active)
{
    if (_eyesInstance)
        getEyes(_eyesInstance)->setSweat(active);
}
