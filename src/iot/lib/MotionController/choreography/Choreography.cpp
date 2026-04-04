#include "Choreography.h"

/**
 * @section Ciclo de Vida e Setup
 */

ChoreoSequence::ChoreoSequence()
    : _currentStepIndex(0), _timer(0.0f), _isActive(false), _isLooping(true),
      _isFirstFrame(true)
{
}

void ChoreoSequence::setFrames(const std::vector<ChoreoStep>& steps)
{
    _steps = steps;
}

/**
 * @section Controle de Reprodução
 */

void ChoreoSequence::play(float startDelay, bool loop)
{
    if (_steps.empty())
        return;

    _isActive = true;
    _isLooping = loop;
    _currentStepIndex = 0;
    _timer = -startDelay;
    _isFirstFrame = true;
}

void ChoreoSequence::stop()
{
    _isActive = false;
    _timer = 0.0f;
    _isFirstFrame = true;
}

/**
 * @section Motor de Execução
 */

const ChoreoStep* ChoreoSequence::update(float deltaTime)
{
    if (!_isActive || _steps.empty())
        return nullptr;

    _timer += deltaTime;

    if (_timer < 0)
        return nullptr;

    if (_isFirstFrame)
    {
        _isFirstFrame = false;
        return &_steps[_currentStepIndex];
    }

    const ChoreoStep& currentStep = _steps[_currentStepIndex];

    if (_timer >= currentStep.duration)
    {
        _timer -= currentStep.duration;

        if (!_isLooping && _currentStepIndex == _steps.size() - 1)
        {
            _isActive = false;
            return nullptr;
        }

        _currentStepIndex = (_currentStepIndex + 1) % _steps.size();
        return &_steps[_currentStepIndex];
    }

    return nullptr;
}

bool ChoreoSequence::isActive() const { return _isActive; }
