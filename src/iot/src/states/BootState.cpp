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

/** @section Ciclo de Vida */

void BootState::enter(GameController* controller)
{
    // O áudio será colocado em buffer e tocará assim que o hardware estiver
    // pronto (~3.5s)
    controller->getAudio().playFile(AudioFiles::BOOT);

    // Centraliza todos os servos usando o motor de comportamentos para evitar
    // trancos elétricos.
    controller->getBehaviors().setPool(BOOT_POOL, 0, 0, false);
}

void BootState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void BootState::update(GameController* controller)
{
    // Aguarda o tempo configurado para garantir que o hardware estabilizou.
    if (millis() - controller->getStateStartTime() > GameConfig::BOOT_DELAY_MS)
    {
        controller->changeState(RobotState::IDLE);
    }
}

/** @section Tratamento de Eventos */

void BootState::handleRFID(GameController* controller, const String& uid) {}
