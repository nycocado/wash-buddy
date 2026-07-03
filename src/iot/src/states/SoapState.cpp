#include "states/SoapState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> SOAP_POOL = {BehaviorVignette(
    eEmotions::Focused,
    0.0f,
    1.0f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::soapSlide(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::soapSlide(), 0.8f, true), // Perfectly
                                                                // alternated
    GameConfig::SOAP_TIMEOUT
)};

/** @section Lifecycle */

void SoapState::enter(GameController* controller)
{
    // Pedagogical logic: computes how many times the playlist fits within
    // the stage's time.
    uint8_t maxLoops =
        GameConfig::SOAP_TIMEOUT / Playlists::SOAP.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Sequential sound feedback for SOAP (bubbles)
    controller->getAudio().playSequence(Playlists::SOAP, maxLoops);

    // Hygiene visual effect
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Runs a "grabbing soap" motion, alternating the arms.
    controller->getBehaviors().setPool(SOAP_POOL, 0, 0, false);
}

void SoapState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void SoapState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Event Handling */

void SoapState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::SOAP, RFIDTags::SCRUB, RobotState::SCRUB
    );
}
