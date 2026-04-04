#include "ParticleSystem.h"

/**
 * @section Ciclo de Vida
 */

ParticleSystem::ParticleSystem(
    U8G2& display,
    const ParticleSystemConfig& config
)
    : _display(display), _config(config), _currentEffect(EffectType::NONE)
{
    // Inicializa todos os slots de partículas como vazios
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        _particles[i] = nullptr;
    }
}

ParticleSystem::~ParticleSystem() { clearAll(); }

/**
 * @section Gestão de Efeitos
 */

void ParticleSystem::setEffect(EffectType type) { _currentEffect = type; }

void ParticleSystem::update(float deltaTime)
{
    int activeCount = 0;

    // Primeiro pass: conta quantas partículas estão vivas no momento
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            activeCount++;
        }
    }

    // Segundo pass: atualiza partículas existentes e cria novas se necessário
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] == nullptr)
        {
            // Se o slot está vazio, tenta criar uma nova partícula baseada na
            // chance de spawn
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

        // Atualiza a física da partícula. Se retornar false, ela morreu (saiu
        // da tela ou estourou)
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
    // Renderiza cada partícula ativa usando o buffer único do display
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            _particles[i]->draw(_display);
        }
    }
}

/**
 * @section Lógica Interna e Memória
 */

void ParticleSystem::spawnParticle(int i)
{
    // Proteção para não sobrescrever uma partícula ativa no slot
    if (_particles[i] != nullptr)
    {
        return;
    }

    // A maioria das partículas nasce em uma posição X aleatória no topo ou base
    float startX = (float)random(0, _config.screenWidth);

    // --- FÁBRICA DE PARTÍCULAS ---
    // Instancia o tipo de partícula correspondente ao efeito ativo.
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

        case EffectType::SPARKLE:
            _particles[i] = new SparkleParticle(
                startX, (float)_config.screenHeight, _sparkleConfig
            );
            break;

        default:
            break;
    }
}

void ParticleSystem::clearAll()
{
    // Limpeza rigorosa de memória para evitar leaks ao mudar de estado ou
    // desligar
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (_particles[i] != nullptr)
        {
            delete _particles[i];
            _particles[i] = nullptr;
        }
    }
}
