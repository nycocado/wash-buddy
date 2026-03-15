#include "Choreography.h"

ChoreoSequence::ChoreoSequence()
    : _currentStepIndex(0), _timer(0.0f), _isActive(false), _isLooping(true)
{
}

void ChoreoSequence::setFrames(const std::vector<ChoreoStep>& steps)
{
    // Define o conjunto de passos que compõem esta coreografia específica.
    _steps = steps;
}

void ChoreoSequence::play(float startDelay, bool loop)
{
    if (_steps.empty())
    {
        return;
    }

    _isActive = true;
    _isLooping = loop;
    _currentStepIndex = 0;

    // O timer começa negativo se houver um delay inicial.
    // Isso atrasa o início da primeira ação da sequência.
    _timer = -startDelay;
}

void ChoreoSequence::stop()
{
    _isActive = false;
    _timer = 0.0f;
}

bool ChoreoSequence::isActive() const { return _isActive; }

const ChoreoStep* ChoreoSequence::update(float deltaTime)
{
    // Se a sequência não estiver rodando ou estiver vazia, ignora o update.
    if (!_isActive || _steps.empty())
    {
        return nullptr;
    }

    _timer += deltaTime;

    // Se ainda estiver no período de delay, nada acontece.
    if (_timer < 0)
    {
        return nullptr;
    }

    const ChoreoStep& currentStep = _steps[_currentStepIndex];

    // --- CONTROLE DE TEMPO POR PASSO ---
    // Verifica se o tempo decorrido atingiu a duração definida para este passo.
    if (_timer >= currentStep.duration)
    {
        _timer = 0.0f; // Reinicia o timer para o próximo passo.

        // Se a coreografia não for cíclica e chegarmos ao último passo,
        // paramos.
        if (!_isLooping && _currentStepIndex == _steps.size() - 1)
        {
            _isActive = false;
            return nullptr;
        }

        // Avança para o próximo passo (ou volta para o primeiro se for loop).
        _currentStepIndex = (_currentStepIndex + 1) % _steps.size();
        return &_steps[_currentStepIndex];
    }

    // Retorna o primeiro passo imediatamente assim que o delay inicial acaba.
    if (_timer == deltaTime && _currentStepIndex == 0)
    {
        return &_steps[_currentStepIndex];
    }

    // Retorna nullptr indicando que o motor deve continuar movendo o servo
    // para o alvo do passo atual, mas sem novas mudanças de comando.
    return nullptr;
}
