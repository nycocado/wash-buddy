#include "states/DryState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> DRY_POOL = {
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900

    ),
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        0.1f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900
    ),
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        -0.1f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900
    )
};

/** @section Lifecycle */

void DryState::enter(GameController* controller)
{
    // Pedagogical logic: computes how many times the playlist fits within
    // the stage's time to avoid silence or abrupt cuts.
    uint8_t maxLoops = GameConfig::DRY_TIMEOUT / Playlists::DRY.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Sequential sound feedback for DRY (wind)
    controller->getAudio().playSequence(Playlists::DRY, maxLoops);

    // Atmospheric visual effect
    controller->getDisplay().setParticleEffect(EffectType::WIND);

    // Runs the "puffing" vibration throughout the drying ritual.
    controller->getBehaviors().setPool(DRY_POOL, 0, 0, true);
}

void DryState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void DryState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        // Ritual successfully completed.
        controller->changeState(RobotState::SUCCESS);
    }
}

/** @section Event Handling */

void DryState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::TOWEL, nullptr, RobotState::SUCCESS
    );
}
