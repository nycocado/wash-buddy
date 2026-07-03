#include "ParticleSystem.h"

/**
 * @section Lifecycle
 */

ParticleSystem::ParticleSystem(
    U8G2& display,
    const ParticleSystemConfig& config
)
    : _display(display), _config(config), _currentEffect(EffectType::NONE)
{
    // Initializes every particle slot as empty
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        _particles[i] = nullptr;
    }
}

ParticleSystem::~ParticleSystem() { clearAll(); }

/**
 * @section Effect Management
 */

void ParticleSystem::setEffect(EffectType type) { _currentEffect = type; }

void ParticleSystem::update(float deltaTime)
{
    int activeCount = 0;

    // First pass: counts how many particles are currently alive
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            activeCount++;
        }
    }

    // Second pass: updates existing particles and spawns new ones if
    // needed
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] == nullptr)
        {
            // If the slot is empty, tries to spawn a new particle based on
            // the spawn chance
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

        // Updates the particle's physics. If it returns false, it died
        // (left the screen or popped)
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

void ParticleSystem::draw()
{
    // Renders each active particle using the display's single buffer
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            _particles[i]->draw(_display);
        }
    }
}

/**
 * @section Internal Logic and Memory
 */

void ParticleSystem::spawnParticle(int i)
{
    // Guard to avoid overwriting an active particle in the slot
    if (_particles[i] != nullptr)
    {
        return;
    }

    // Most particles are born at a random X position at the top or bottom
    float startX = (float)random(0, _config.screenWidth);

    // --- PARTICLE FACTORY ---
    // Instantiates the particle type matching the active effect.
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

void ParticleSystem::clearAll()
{
    // Rigorous memory cleanup to avoid leaks when changing state or
    // shutting down
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            delete _particles[i];
            _particles[i] = nullptr;
        }
    }
}
