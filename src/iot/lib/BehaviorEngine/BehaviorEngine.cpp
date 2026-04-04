#include "BehaviorEngine.h"
#include "AudioController.h"
#include "DisplayOrchestrator.h"
#include "MotionController.h"

/**
 * @section Ciclo de Vida e Configuração
 */

BehaviorEngine::BehaviorEngine() {}

void BehaviorEngine::setPool(
    const std::vector<BehaviorVignette>& pool,
    unsigned long minPauseMs,
    unsigned long maxPauseMs,
    bool loop,
    bool resetOnRest
)
{
    _pool = pool;
    _minPause = minPauseMs;
    _maxPause = maxPauseMs;
    _loop = loop;
    _resetOnRest = resetOnRest;
    _currentIndex = -1;
    _isInAction = false;

    // Agenda o primeiro comportamento para acontecer após uma pequena pausa
    // inicial para evitar disparos imediatos durante transições de estado.
    _nextEventTime = millis() + _minPause;

    Serial.println(F("[BEHAVIOR] Novo pool de comportamentos carregado."));
}

void BehaviorEngine::stop()
{
    _pool.clear();
    _currentIndex = -1;
    _isInAction = false;
    _nextEventTime = 0;
    _resetOnRest = false;
}

/**
 * @section Motor de Execução (Update Loop)
 */

void BehaviorEngine::update(
    DisplayOrchestrator& display,
    MotionController& motion,
    AudioController& audio
)
{
    // O motor silencia se não houver ações ou se estiver em pausa (ex: durante
    // instruções)
    if (_pool.empty() || _isPaused)
        return;

    unsigned long now = millis();

    // Lógica de alternância entre Ação (Vignette) e Repouso (Rest)
    if (now >= _nextEventTime)
    {
        if (!_isInAction)
        {
            pickNextAction(display, motion, audio);
        }
        else
        {
            enterRest(display, motion);
        }
    }
}

/**
 * @section Gestão de Vinhetas e Silêncio Orgânico
 */

void BehaviorEngine::pickNextAction(
    DisplayOrchestrator& display,
    MotionController& motion,
    AudioController& audio
)
{
    if (_pool.empty())
        return;

    // Escolha do índice baseada no modo de reprodução
    if (_loop)
    {
        // No modo loop, sorteamos aleatoriamente para simular comportamento
        // orgânico e imprevisível.
        _currentIndex = random(0, _pool.size());
    }
    else
    {
        // No modo sequencial (One-Shot), avançamos linearmente.
        _currentIndex++;
        if (_currentIndex >= (int)_pool.size())
        {
            stop();
            return;
        }
    }

    BehaviorVignette& vignette = _pool[_currentIndex];
    _isInAction = true;

    // --- ORQUESTRAÇÃO DA VINHETA ---

    // 1. Expressão Facial e Direção do Olhar
    display.setEyeMood(vignette.mood);
    display.lookAt(vignette.lookX, vignette.lookY);

    // 2. Acionamento de Coreografias (Cérebro Motor)
    if (!vignette.head.steps.empty())
    {
        motion.playHeadChoreography(
            vignette.head.steps, vignette.head.delay, vignette.head.loop
        );
    }

    if (!vignette.armL.steps.empty())
    {
        motion.playArmLChoreography(
            vignette.armL.steps, vignette.armL.delay, vignette.armL.loop
        );
    }

    if (!vignette.armR.steps.empty())
    {
        motion.playArmRChoreography(
            vignette.armR.steps, vignette.armR.delay, vignette.armR.loop
        );
    }

    // 3. Disparo de Áudio Sincronizado
    if (vignette.audioTrack.id > 0)
    {
        audio.playFile(vignette.audioTrack);
    }

    // Define o timer para o fim desta ação (duração pedagógica da vinheta)
    _nextEventTime = millis() + vignette.durationMs;
}

void BehaviorEngine::enterRest(
    DisplayOrchestrator& display,
    MotionController& motion
)
{
    _isInAction = false;

    // Garante que qualquer animação de braço/cabeça da vinheta anterior seja
    // cessada
    motion.stopAllAnimations();

    // --- RESET VISUAL (Opcional) ---
    // Útil para estados de IDLE onde queremos que o robô volte ao neutro entre
    // acenos.
    if (_resetOnRest)
    {
        display.setEyeMood(eEmotions::Normal);
        display.lookAt(0.0f, 0.0f);
        motion.centerAll();
    }

    // Sorteia o tempo de silêncio/repouso entre as vinhetas para evitar
    // roboticidade.
    unsigned long pause = random(_minPause, _maxPause + 1);
    _nextEventTime = millis() + pause;
}
