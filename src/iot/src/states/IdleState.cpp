#include "states/IdleState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Defines the personality pool for the resting (Idle) mode
static const std::vector<BehaviorVignette> IDLE_POOL = {
    BehaviorVignette(
        eEmotions::Focused,
        0.0f,
        0.2f,
        ChoreoAction(ChoreographyLibrary::idlePanLook()),
        ChoreoAction(),
        ChoreoAction(),
        6300 // Total time of idlePanLook
    ),
    BehaviorVignette(
        eEmotions::Glee,
        0.0f,
        0.0f,
        ChoreoAction(),
        ChoreoAction(ChoreographyLibrary::waveHand(), 0.0f, true),
        ChoreoAction(),
        8500,
        AudioFiles::HIDE_AND_BEAK
    ),
    BehaviorVignette(
        eEmotions::Glee,
        0.0f,
        0.0f,
        ChoreoAction(),
        ChoreoAction(),
        ChoreoAction(ChoreographyLibrary::waveHand(), 0.0f, true),
        8500,
        AudioFiles::HIDE_AND_BEAK
    ),
    BehaviorVignette(
        eEmotions::Surprised,
        0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(70)),
        ChoreoAction(),
        ChoreoAction(),
        2000 // Total time of lookSide
    ),
    BehaviorVignette(
        eEmotions::Surprised,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(110)),
        ChoreoAction(),
        ChoreoAction(),
        2000 // Total time of lookSide
    )
};

/** @section Lifecycle */

void IdleState::enter(GameController* controller)
{
    // "Ready" sound feedback, unless coming back from WAITING (timeout) to
    // avoid being repetitive.
    State* prev = controller->getPreviousState();
    if (prev == nullptr || prev->getStateEnum() != RobotState::WAITING)
    {
        controller->getAudio().playFile(AudioFiles::IDLE_READY);
        // Initial happiness animation (Glee includes a swaying motion)
        controller->getDisplay().setEyeMood(eEmotions::Glee);
    }

    // --- PERSONALITY SETUP ---
    // Sets up the pool with long NATURAL PAUSES (8s to 15s) and enables the
    // visual reset so the robot returns to neutral between vignettes.
    controller->getBehaviors().setPool(
        IDLE_POOL,
        GameConfig::IDLE_MIN_PAUSE_MS,
        GameConfig::IDLE_MAX_PAUSE_MS,
        true, // Loop enabled
        true  // ResetOnRest enabled for the Idle mode
    );

    // First visual reminder after the configured initial delay.
    _lastReminderTime = millis() + GameConfig::IDLE_INITIAL_DELAY_MS;
}

void IdleState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void IdleState::update(GameController* controller)
{
    DisplayOrchestrator& display = controller->getDisplay();
    BehaviorEngine& behaviors = controller->getBehaviors();
    unsigned long now = millis();

    // --- TIMEOUT FOR POWER SAVING ---
    // If nobody interacts with the robot for IDLE_TIMEOUT, it shuts down.
    if (now - controller->getStateStartTime() > GameConfig::IDLE_TIMEOUT_MS)
    {
        controller->shutdownSystem();
        return;
    }

    // If there's an instruction on screen, pause the reminder logic.
    if (display.isInstructionActive())
    {
        return;
    }

    // ORGANIC REMINDER LOGIC:
    // Only trigger the instruction if the time has come and the robot is
    // at rest.
    if (now > _lastReminderTime && !behaviors.isInAction())
    {
        // Reminder sound feedback (BEW-WOO BEW-WOO)
        controller->getAudio().playFile(AudioFiles::IDLE_REMINDER);

        // The orchestrator's "Zipper" effect will handle the smooth visual
        // transition.
        display.showInstruction(
            Assets::ICON_FAUCET, GameConfig::INSTRUCTION_DISPLAY_MS
        );

        // Schedules the next reminder at the configured interval.
        _lastReminderTime = now + GameConfig::IDLE_REMINDER_INTERVAL_MS;
    }
}

/** @section Event Handling */

void IdleState::handleRFID(GameController* controller, const String& uid)
{
    if (uid == RFIDTags::FAUCET)
    {
        controller->changeState(RobotState::WET);
    }
    else
    {
        // Any other tag while in IDLE is treated as a sequence error.
        controller->changeState(RobotState::ERROR);
    }
}
