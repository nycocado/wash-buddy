#include "states/ErrorState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> ERROR_POOL = {BehaviorVignette(
    eEmotions::Annoyed,
    0.0f,
    0.0f,
    ChoreoAction(ChoreographyLibrary::shakeHeadError()),
    ChoreoAction({{120, GameConfig::ERROR_DISPLAY_MS / 1000.0f}}),
    ChoreoAction({{120, GameConfig::ERROR_DISPLAY_MS / 1000.0f}}),
    GameConfig::ERROR_DISPLAY_MS
)};

/** @section Lifecycle */

void ErrorState::enter(GameController* controller)
{
    // Error sound feedback (NO)
    controller->getAudio().playFile(AudioFiles::ERROR);

    // Confused/doubtful facial expression
    controller->getDisplay().playConfused();

    // Runs a custom behavior through BehaviorEngine in a single loop.
    controller->getBehaviors().setPool(ERROR_POOL, 0, 0, false);
}

void ErrorState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void ErrorState::update(GameController* controller)
{
    // After showing the error for the configured time, decides where to
    // return.
    if (millis() - controller->getStateStartTime() >
        GameConfig::ERROR_DISPLAY_MS)
    {
        State* prev = controller->getPreviousState();

        // If the error happened during the ritual, goes back to WAITING so
        // the child can try the correct stage again.
        if (prev != nullptr && prev->getStateEnum() != RobotState::IDLE &&
            prev->getStateEnum() != RobotState::BOOT)
        {
            controller->changeState(RobotState::WAITING);
        }
        else
        {
            // If the mistake happened right at the start (Idle), returns
            // to rest.
            controller->changeState(RobotState::IDLE);
        }
    }
}

/** @section Event Handling */

void ErrorState::handleRFID(GameController* controller, const String& uid) {}
