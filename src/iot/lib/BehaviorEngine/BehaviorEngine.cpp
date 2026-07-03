#include "BehaviorEngine.h"
#include "AudioController.h"
#include "DisplayOrchestrator.h"
#include "MotionController.h"

/**
 * @section Lifecycle and Configuration
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

    // Schedules the first behavior to happen after a small initial pause,
    // to avoid immediate triggers during state transitions.
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
 * @section Execution Engine (Update Loop)
 */

void BehaviorEngine::update(
    DisplayOrchestrator& display,
    MotionController& motion,
    AudioController& audio
)
{
    // The engine goes silent if there are no actions or if it's paused
    // (e.g. during instructions)
    if (_pool.empty() || _isPaused)
        return;

    unsigned long now = millis();

    // Alternates between action (vignette) and rest logic
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
 * @section Vignette Management and Organic Silence
 */

void BehaviorEngine::pickNextAction(
    DisplayOrchestrator& display,
    MotionController& motion,
    AudioController& audio
)
{
    if (_pool.empty())
        return;

    // Index selection based on the playback mode
    if (_loop)
    {
        // In loop mode, we pick randomly to simulate organic,
        // unpredictable behavior.
        _currentIndex = random(0, _pool.size());
    }
    else
    {
        // In sequential (one-shot) mode, we advance linearly.
        _currentIndex++;
        if (_currentIndex >= (int)_pool.size())
        {
            stop();
            return;
        }
    }

    BehaviorVignette& vignette = _pool[_currentIndex];
    _isInAction = true;

    // --- VIGNETTE ORCHESTRATION ---

    // 1. Facial expression and look direction
    display.setEyeMood(vignette.mood);
    display.lookAt(vignette.lookX, vignette.lookY);

    // 2. Choreography triggers (motor brain)
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

    // 3. Synchronized audio trigger
    if (vignette.audioTrack.id > 0)
    {
        audio.playFile(vignette.audioTrack);
    }

    // Sets the timer for the end of this action (the vignette's
    // pedagogical duration)
    _nextEventTime = millis() + vignette.durationMs;
}

void BehaviorEngine::enterRest(
    DisplayOrchestrator& display,
    MotionController& motion
)
{
    _isInAction = false;

    // Ensures any arm/head animation from the previous vignette is
    // stopped
    motion.stopAllAnimations();

    // --- VISUAL RESET (optional) ---
    // Useful for IDLE states, where we want the robot to return to
    // neutral between waves.
    if (_resetOnRest)
    {
        display.setEyeMood(eEmotions::Normal);
        display.lookAt(0.0f, 0.0f);
        motion.centerAll();
    }

    // Picks a random rest/silence time between vignettes to avoid
    // robotic-feeling motion.
    unsigned long pause = random(_minPause, _maxPause + 1);
    _nextEventTime = millis() + pause;
}
