#include "ParticleManager.h"
#include "ParticleTypes.h"

ParticleManager::ParticleManager(
    Adafruit_SSD1306& display,
    const ParticleManagerConfig& config
)
    : _display(display), _config(config), _currentEffect(EffectType::NONE)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        _particles[i] = nullptr;
    }
}

ParticleManager::~ParticleManager() { clearAll(); }

void ParticleManager::setEffect(EffectType type) { _currentEffect = type; }

void ParticleManager::spawnParticle(int i)
{
    if (_particles[i] != nullptr)
        return;

    float startX = (float)random(0, _config.screenWidth);

    switch (_currentEffect)
    {
        case EffectType::BUBBLES:
            _particles[i] =
                new BubbleParticle(startX, _config.screenHeight, _bubbleConfig);
            break;
        case EffectType::RAIN_LIGHT:
            _particles[i] = new RainParticle(startX, -10, _rainConfig, false);
            break;
        case EffectType::RAIN_HEAVY:
            _particles[i] = new RainParticle(startX, -15, _rainConfig, true);
            break;
        case EffectType::CONFETTI:
            _particles[i] = new ConfettiParticle(
                _config.screenWidth / 2.0f,
                _config.screenHeight / 2.0f,
                _confettiConfig
            );
            break;
        case EffectType::WIND:
            _particles[i] = new WindParticle(
                -20.0f, (float)random(0, _config.screenHeight), _windConfig
            );
            break;
        default:
            break;
    }
}

void ParticleManager::update(float deltaTime)
{
    int activeCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
            activeCount++;
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] == nullptr)
        {
            if (_currentEffect != EffectType::NONE &&
                activeCount < _config.activeLimit)
            {
                if (random(0, 100) < _config.spawnChance)
                {
                    spawnParticle(i);
                    activeCount++;
                }
            }
            continue;
        }

        if (!_particles[i]->update(
                deltaTime, _config.screenWidth, _config.screenHeight
            ))
        {
            delete _particles[i];
            _particles[i] = nullptr;
            activeCount--;
        }
    }
}

void ParticleManager::draw()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            _particles[i]->draw(_display);
        }
    }
}

void ParticleManager::clearAll()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            delete _particles[i];
            _particles[i] = nullptr;
        }
    }
}
