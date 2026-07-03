#include "states/ScrubState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> SCRUB_POOL = {BehaviorVignette(
    eEmotions::Glee,
    0.0f,
    -0.5f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::scrubArm(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::scrubArm(), 0.4f, true),
    GameConfig::SCRUB_TIMEOUT
)};

/** @section Lifecycle */

void ScrubState::enter(GameController* controller)
{
    // Pedagogical logic: computes how many times the playlist fits within
    // the stage's time.
    uint8_t maxLoops =
        GameConfig::SCRUB_TIMEOUT / Playlists::SCRUB.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Sequential sound feedback for SCRUB
    controller->getAudio().playSequence(Playlists::SCRUB, maxLoops);

    // Hygiene visual effect
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Runs a repetitive, synchronized scrubbing motion between the arms.
    controller->getBehaviors().setPool(SCRUB_POOL, 0, 0, false);
}

void ScrubState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void ScrubState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Event Handling */

void ScrubState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::SCRUB, RFIDTags::FAUCET, RobotState::RINSE
    );
}
