#include "states/RinseState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> RINSE_POOL = {BehaviorVignette(
    eEmotions::Happy,
    0.0f,
    -0.5f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.2f, true),
    GameConfig::RINSE_TIMEOUT
)};

/** @section Lifecycle */

void RinseState::enter(GameController* controller)
{
    // Pedagogical logic: computes how many times the playlist fits within
    // the stage's time.
    uint8_t maxLoops =
        GameConfig::RINSE_TIMEOUT / Playlists::RINSE.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Sequential sound feedback for RINSE
    controller->getAudio().playSequence(Playlists::RINSE, maxLoops);

    // Heavy rain visual effect
    controller->getDisplay().setParticleEffect(EffectType::RAIN_HEAVY);

    // "Dipping" motion simulating the hands under the water.
    controller->getBehaviors().setPool(RINSE_POOL, 0, 0, false);
}

void RinseState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void RinseState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Event Handling */

void RinseState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::TOWEL, RobotState::DRY
    );
}
