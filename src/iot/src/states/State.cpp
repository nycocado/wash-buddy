#include "states/State.h"
#include "GameController.h"

/** @section Event Handling */

void State::validateRFID(
    GameController* controller,
    const String& uid,
    const char* currentTag,
    const char* nextTag,
    RobotState nextState
)
{
    // CASE 1: the tag that was read is the same one that started the
    // current state. This is interpreted as an attempt to repeat or restart
    // the current stage.
    if (uid == currentTag)
    {
        controller->handleRepeat();
    }
    // CASE 2: the tag that was read is the expected tag for the NEXT stage
    // of the ritual. Allows the ritual to advance to the next logical
    // state.
    else if (nextTag != nullptr && uid == nextTag)
    {
        controller->changeState(nextState);
    }
    // CASE 3: any other tag was read (wrong order or unknown tag).
    // Transitions to the error state, indicating the child should pay
    // attention.
    else
    {
        controller->changeState(RobotState::ERROR);
    }
}
