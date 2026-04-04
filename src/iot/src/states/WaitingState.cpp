#include "states/WaitingState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Pools de Comportamento para o estado de Espera
static const std::vector<BehaviorVignette> WAITING_WORRIED_POOL = {
    // Procura para a direita
    BehaviorVignette(
        eEmotions::Worried,
        0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(120), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        3000
    ),
    // Procura para a esquerda
    BehaviorVignette(
        eEmotions::Worried,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(60), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        3000
    ),
    // Foca na frente
    BehaviorVignette(
        eEmotions::Worried,
        0.0f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::idlePanLook(), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        6300
    )};

static const std::vector<BehaviorVignette> WAITING_SAD_POOL = {
    // Triste olhando para baixo
    BehaviorVignette(
        eEmotions::Sad,
        0.0f,
        -0.5f,
        ChoreoAction(ChoreographyLibrary::lookSide(90), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        4000
    ),
    // Triste olhando um pouco para o lado
    BehaviorVignette(
        eEmotions::Sad,
        0.3f,
        -0.3f,
        ChoreoAction(ChoreographyLibrary::lookSide(100), 0.0f),
        ChoreoAction(),
        ChoreoAction(),
        4000
    )};

/** @section Ciclo de Vida */

void WaitingState::enter(GameController* controller)
{
    _isSadPhase = false;
    _isWorriedPhase = false;
    _waitingAudioPlayed = false;
    _sadAudioPlayed = false;

    // --- INSTRUÇÃO VISUAL ---
    // Determina qual ícone de instrução mostrar baseado no progresso do ritual.
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
        // Se não há ícone pendente, o áudio já pode tocar (ex: erro no início)
        _waitingAudioPlayed = true;
        controller->getAudio().playFile(AudioFiles::WAITING_START);
    }
}

void WaitingState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void WaitingState::update(GameController* controller)
{
    DisplayOrchestrator& display = controller->getDisplay();
    unsigned long elapsed = millis() - controller->getStateStartTime();

    // --- GATILHO DE ÁUDIO SINCRONIZADO ---
    // O áudio inicial (ZWEE?) só toca quando a carinha (olhos) volta a
    // aparecer, ou seja, quando o ícone de instrução inicial desaparece.
    if (!_waitingAudioPlayed && !display.isInstructionActive())
    {
        controller->getAudio().playFile(AudioFiles::WAITING_START);
        _waitingAudioPlayed = true;
    }

    // --- LÓGICA DE EXPIRAÇÃO (TIMEOUT) ---
    if (elapsed > GameConfig::WAITING_TIMEOUT_MS)
    {
        // Feedback sonoro de "desistência" (OOOOOOOOOOOOOO...)
        controller->getAudio().playFile(AudioFiles::WAITING_OFF);
        controller->shutdownSystem();
        return;
    }

    // --- LEMBRETE VISUAL DA PRÓXIMA ETAPA ---
    unsigned long now = millis();
    if (_nextIcon != nullptr &&
        now - _lastReminderTime > GameConfig::WAITING_REMINDER_INTERVAL_MS)
    {
        controller->getAudio().playFile(AudioFiles::IDLE_REMINDER);
        display.showInstruction(_nextIcon, 5000);
        _lastReminderTime = now;
    }

    // --- EVOLUÇÃO DA IMPACIÊNCIA ---
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
        // Feedback sonoro de tristeza (W-W-WOOOOO..?)
        controller->getAudio().playFile(AudioFiles::WAITING_15S);
        _sadAudioPlayed = true;
    }
}

/** @section Tratamento de Eventos */

void WaitingState::handleRFID(GameController* controller, const String& uid)
{
    // Recupera onde o ritual parou para saber qual a próxima tag válida
    RobotState lastRitual = controller->getLastRitualState();

    // Segurança: se não houve ritual ainda, volta para o IDLE
    if (lastRitual == RobotState::BOOT || lastRitual == RobotState::IDLE)
    {
        controller->changeState(RobotState::IDLE);
        return;
    }

    // Lógica de transição baseada na última etapa concluída
    if (lastRitual == RobotState::WET) // Parou em: Molhar as mãos
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::WET); // Repetir
        else if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP); // Avançar para Sabão
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SOAP) // Parou em: Passar sabão
    {
        if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP);
        else if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB);
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SCRUB) // Parou em: Esfregar
    {
        if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB);
        else if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE);
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::RINSE) // Parou em: Enxaguar
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE);
        else if (uid == RFIDTags::TOWEL)
            controller->changeState(RobotState::DRY);
        else
            controller->changeState(RobotState::ERROR);
    }
}
