#include "states/WaitingState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Behavior pools for the waiting state
static const std::vector<BehaviorVignette> WAITING_WORRIED_POOL = {
    // Looks to the right
    BehaviorVignette(
        eEmotions::Worried,
        0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(120), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        3000
    ),
    // Looks to the left
    BehaviorVignette(
        eEmotions::Worried,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(60), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        3000
    ),
    // Focuses ahead
    BehaviorVignette(
        eEmotions::Worried,
        0.0f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::idlePanLook(), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        6300
    )
};

static const std::vector<BehaviorVignette> WAITING_SAD_POOL = {
    // Sad, looking down
    BehaviorVignette(
        eEmotions::Sad,
        0.0f,
        -0.5f,
        ChoreoAction(ChoreographyLibrary::lookSide(90), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        4000
    ),
    // Sad, looking slightly to the side
    BehaviorVignette(
        eEmotions::Sad,
        0.3f,
        -0.3f,
        ChoreoAction(ChoreographyLibrary::lookSide(100), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        4000
    )
};

/** @section Lifecycle */

void WaitingState::enter(GameController* controller)
{
    _isSadPhase = false;
    _isWorriedPhase = false;
    _waitingAudioPlayed = false;
    _sadAudioPlayed = false;

    // --- VISUAL INSTRUCTION ---
    // Determines which instruction icon to show based on the ritual's
    // progress.
    RobotState lastRitual = controller->getLastRitualState();
    _nextIcon = nullptr;

    if (lastRitual == RobotState::WET)
        _nextIcon = Assets::ICON_SOAP;
    else if (lastRitual == RobotState::SOAP)
        _nextIcon = Assets::ICON_SCRUB;
    else if (lastRitual == RobotState::SCRUB)
        _nextIcon = Assets::ICON_RINSE;
    else if (lastRitual == RobotState::RINSE)
        _nextIcon = Assets::ICON_TOWEL;

    if (_nextIcon != nullptr)
    {
        controller->getAudio().playFile(AudioFiles::IDLE_REMINDER);
        controller->getDisplay().showInstruction(_nextIcon, 5000);
        _lastReminderTime = millis();
    }
    else
    {
        // If there's no pending icon, the audio can already play (e.g.
        // error right at the start)
        _waitingAudioPlayed = true;
        controller->getAudio().playFile(AudioFiles::WAITING_START);
    }
}

void WaitingState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Logic Update */

void WaitingState::update(GameController* controller)
{
    DisplayOrchestrator& display = controller->getDisplay();
    unsigned long elapsed = millis() - controller->getStateStartTime();

    // --- SYNCHRONIZED AUDIO TRIGGER ---
    // The initial audio only plays once the face (eyes) reappears, i.e.
    // once the initial instruction icon disappears.
    if (!_waitingAudioPlayed && !display.isInstructionActive())
    {
        controller->getAudio().playFile(AudioFiles::WAITING_START);
        _waitingAudioPlayed = true;
    }

    // --- EXPIRATION LOGIC (TIMEOUT) ---
    if (elapsed > GameConfig::WAITING_TIMEOUT_MS)
    {
        // "Giving up" sound feedback
        controller->getAudio().playFile(AudioFiles::WAITING_OFF);
        controller->shutdownSystem();
        return;
    }

    // --- VISUAL REMINDER OF THE NEXT STAGE ---
    unsigned long now = millis();
    if (_nextIcon != nullptr &&
        now - _lastReminderTime > GameConfig::WAITING_REMINDER_INTERVAL_MS)
    {
        controller->getAudio().playFile(AudioFiles::IDLE_REMINDER);
        display.showInstruction(_nextIcon, 5000);
        _lastReminderTime = now;
    }

    // --- IMPATIENCE EVOLUTION ---
    if (!_isWorriedPhase && elapsed > 5000)
    {
        _isWorriedPhase = true;
        controller->getBehaviors().setPool(WAITING_WORRIED_POOL);
    }

    if (!_isSadPhase && elapsed > GameConfig::WAITING_SAD_PHASE_MS)
    {
        _isSadPhase = true;
        controller->getBehaviors().setPool(WAITING_SAD_POOL);
    }

    if (_isSadPhase && !_sadAudioPlayed && !display.isInstructionActive())
    {
        // Sad sound feedback
        controller->getAudio().playFile(AudioFiles::WAITING_15S);
        _sadAudioPlayed = true;
    }
}

/** @section Event Handling */

void WaitingState::handleRFID(GameController* controller, const String& uid)
{
    // Retrieves where the ritual stopped to know which is the next valid
    // tag
    RobotState lastRitual = controller->getLastRitualState();

    // Safety: if no ritual has started yet, return to IDLE
    if (lastRitual == RobotState::BOOT || lastRitual == RobotState::IDLE)
    {
        controller->changeState(RobotState::IDLE);
        return;
    }

    // Transition logic based on the last completed stage
    if (lastRitual == RobotState::WET) // Stopped at: wetting the hands
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::WET); // Repeat
        else if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP); // Advance to soap
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SOAP) // Stopped at: applying soap
    {
        if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP);
        else if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB);
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SCRUB) // Stopped at: scrubbing
    {
        if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB);
        else if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE);
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::RINSE) // Stopped at: rinsing
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE);
        else if (uid == RFIDTags::TOWEL)
            controller->changeState(RobotState::DRY);
        else
            controller->changeState(RobotState::ERROR);
    }
}
