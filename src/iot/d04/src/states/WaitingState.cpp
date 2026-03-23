#include "states/WaitingState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Pools de Comportamento para o estado de Espera
static const std::vector<BehaviorVignette> WAITING_WORRIED_POOL = {
    // Procura para a direita
    {eEmotions::Worried, 0.7f, 0.0f, ChoreographyLibrary::lookSide(120), 3000},
    // Procura para a esquerda
    {eEmotions::Worried, -0.7f, 0.0f, ChoreographyLibrary::lookSide(60), 3000},
    // Foca na frente
    {eEmotions::Worried, 0.0f, 0.0f, ChoreographyLibrary::idlePanLook(), 2000}};

static const std::vector<BehaviorVignette> WAITING_SAD_POOL = {
    // Triste olhando para baixo
    {eEmotions::Sad, 0.0f, -0.5f, ChoreographyLibrary::lookSide(90), 4000},
    // Triste olhando um pouco para o lado
    {eEmotions::Sad, 0.3f, -0.3f, ChoreographyLibrary::lookSide(100), 4000}};

/** @section Ciclo de Vida */

void WaitingState::enter(GameController* controller)
{
    _isSadPhase = false;

    // Inicia com o comportamento de procuração/preocupação
    controller->getBehaviors().setPool(WAITING_WORRIED_POOL);

    // Determina qual ícone de instrução mostrar baseado no progresso
    RobotState lastRitual = controller->getLastRitualState();
    const uint8_t* nextIcon = nullptr;

    if (lastRitual == RobotState::WET)
        nextIcon = Assets::ICON_SOAP;
    else if (lastRitual == RobotState::SOAP)
        nextIcon = Assets::ICON_SCRUB;
    else if (lastRitual == RobotState::SCRUB)
        nextIcon = Assets::ICON_RINSE;
    else if (lastRitual == RobotState::RINSE)
        nextIcon = Assets::ICON_TOWEL;

    if (nextIcon != nullptr)
    {
        controller->getDisplay().showInstruction(nextIcon, 5000);
    }
}

void WaitingState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void WaitingState::update(GameController* controller)
{
    unsigned long elapsed = millis() - controller->getStateStartTime();

    // Timeout: Se passar do tempo limite (15s), volta para o IDLE
    if (elapsed > GameConfig::WAITING_TIMEOUT_MS)
    {
        controller->changeState(RobotState::IDLE);
        return;
    }

    // Evolução da impaciência via mudança de Pool
    if (!_isSadPhase && elapsed > 7000)
    {
        _isSadPhase = true;
        // Transita a personalidade para triste/desistente
        controller->getBehaviors().setPool(WAITING_SAD_POOL);
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
            controller->changeState(RobotState::SOAP); // Repetir
        else if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB); // Avançar para Esfregar
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SCRUB) // Parou em: Esfregar
    {
        if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB); // Repetir
        else if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE); // Avançar para Enxágue
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::RINSE) // Parou em: Enxágue
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE); // Repetir
        else if (uid == RFIDTags::TOWEL)
            controller->changeState(RobotState::DRY); // Avançar para Secar
        else
            controller->changeState(RobotState::ERROR);
    }
    else
    {
        controller->changeState(RobotState::ERROR);
    }
}
