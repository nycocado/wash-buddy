#include "states/SuccessState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> SUCCESS_POOL = {BehaviorVignette(
    eEmotions::Happy,
    0.0f,
    0.0f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::celebrateVictory(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::celebrateVictory(), 0.1f, true),
    GameConfig::SUCCESS_DISPLAY_MS
)};

/** @section Lifecycle */

void SuccessState::enter(GameController* controller)
{
    // Success sound feedback (DAH-DAH WEEEE)
    controller->getAudio().playFile(AudioFiles::SUCCESS);

    // Extra visual feedback
    controller->getDisplay().playHappy();

    // Festive confetti effect
    controller->getDisplay().setParticleEffect(EffectType::CONFETTI);

    // Runs the celebration behavior
    controller->getBehaviors().setPool(SUCCESS_POOL, 0, 0, false);
}

void SuccessState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void SuccessState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        // After the success time, shuts down the system by sending the
        // signal to the module.
        controller->shutdownSystem();
    }
}

/** @section Event Handling */

void SuccessState::handleRFID(GameController* controller, const String& uid) {}
