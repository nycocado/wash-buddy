#include "states/BootState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> BOOT_POOL = {BehaviorVignette(
    eEmotions::Normal,
    0.0f,
    0.0f,
    ChoreoAction({{90, 1.0f}}),
    ChoreoAction({{90, 1.5f}}),
    ChoreoAction({{90, 1.5f}}),
    GameConfig::BOOT_DELAY_MS
)};

/** @section Lifecycle */

void BootState::enter(GameController* controller)
{
    // The audio will be buffered and will play as soon as the hardware is
    // ready (~3.5s)
    controller->getAudio().playFile(AudioFiles::BOOT);

    // Centers every servo through the behavior engine to avoid electrical
    // jolts.
    controller->getBehaviors().setPool(BOOT_POOL, 0, 0, false);
}

void BootState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void BootState::update(GameController* controller)
{
    // Waits for the configured time to ensure the hardware has stabilized.
    if (millis() - controller->getStateStartTime() > GameConfig::BOOT_DELAY_MS)
    {
        controller->changeState(RobotState::IDLE);
    }
}

/** @section Event Handling */

void BootState::handleRFID(GameController* controller, const String& uid) {}
