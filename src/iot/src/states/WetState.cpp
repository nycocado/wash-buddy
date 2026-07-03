#include "states/WetState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> WET_POOL = {BehaviorVignette(
    eEmotions::Focused,
    0.0f,
    -0.8f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.2f, true),
    GameConfig::WET_TIMEOUT
)};

/** @section Lifecycle */

void WetState::enter(GameController* controller)
{
    // Pedagogical logic: computes how many times the playlist fits within
    // the stage's time
    uint8_t maxLoops = GameConfig::WET_TIMEOUT / Playlists::WET.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Sequential sound feedback for WET (rain)
    controller->getAudio().playSequence(Playlists::WET, maxLoops);

    controller->getDisplay().setParticleEffect(EffectType::RAIN_LIGHT);
    controller->getBehaviors().setPool(WET_POOL, 0, 0, false);
}

void WetState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void WetState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Event Handling */

void WetState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::SOAP, RobotState::SOAP
    );
}
