#include "BehaviorEngine.h"
#include "DisplayOrchestrator.h"
#include "MotionController.h"

/**
 * @section Ciclo de Vida e Configuração
 */

BehaviorEngine::BehaviorEngine()
    : _currentIndex(-1), _nextEventTime(0), _isPaused(false),
      _isInAction(false), _loop(true)
{
}

void BehaviorEngine::setPool(
    const std::vector<BehaviorVignette>& pool,
    unsigned long minPauseMs,
    unsigned long maxPauseMs,
    bool loop
)
{
    _pool = pool;
    _minPause = minPauseMs;
    _maxPause = maxPauseMs;
    _loop = loop;
    _currentIndex = -1;
    _isInAction = false;

    // Iniciamos sempre em Repouso por um tempo curto (500ms)
    // para dar estabilidade à transição visual de entrada no estado.
    _nextEventTime = millis() + 500;
}

void BehaviorEngine::stop()
{
    _pool.clear();
    _currentIndex = -1;
    _isInAction = false;
    _nextEventTime = 0;
}

/**
 * @section Orquestração de Comportamentos
 */

void BehaviorEngine::update(
    DisplayOrchestrator& display,
    MotionController& motion
)
{
    // Se estiver pausado ou sem ações, não processa nada
    if (_isPaused || _pool.empty())
        return;

    if (millis() > _nextEventTime)
    {
        if (_isInAction)
        {
            // A ação atual terminou.
            if (_loop)
            {
                // Modo Loop: Volta para o repouso antes da próxima ação
                enterRest(display);
            }
            else
            {
                // Modo One-Shot: Ação única acabou, encerra o pool
                // O robô permanece na pose/expressão final desta ação
                _pool.clear();
                _isInAction = false;
            }
        }
        else
        {
            // O período de repouso acabou, sorteia a próxima ação
            pickNextAction(display, motion);
        }
    }
}

/**
 * @section Lógica Interna
 */

void BehaviorEngine::pickNextAction(
    DisplayOrchestrator& display,
    MotionController& motion
)
{
    _isInAction = true;
    int nextIndex;

    // Sorteia um índice garantindo que não repita a última ação (se pool > 1)
    if (_pool.size() > 1)
    {
        do
        {
            nextIndex = random(0, _pool.size());
        } while (nextIndex == _currentIndex);
    }
    else
    {
        nextIndex = 0;
    }

    _currentIndex = nextIndex;
    const BehaviorVignette& action = _pool[_currentIndex];

    // --- APLICAÇÃO DOS COMANDOS DA VINHETA ---
    // 1. Define a expressão facial
    display.setEyeMood(action.mood);
    // 2. Define a direção do olhar
    display.lookAt(action.lookX, action.lookY);

    // 3. Inicia a coreografia física, se existir
    if (!action.choreography.empty())
    {
        motion.playHeadChoreography(action.choreography, 0.0f, false);
    }

    // Define o timer para o fim desta ação
    _nextEventTime = millis() + action.durationMs;
}

void BehaviorEngine::enterRest(DisplayOrchestrator& display)
{
    _isInAction = false;

    // Retorna ao normal e centro de forma limpa entre as ações.
    // Isso evita que o robô pareça "travado" em uma expressão tensa por muito
    // tempo.
    display.setEyeMood(eEmotions::Normal);
    display.lookAt(0.0f, 0.0f);

    // Sorteia o tempo de silêncio/repouso entre as vinhetas
    unsigned long pause = random(_minPause, _maxPause + 1);
    _nextEventTime = millis() + pause;
}
