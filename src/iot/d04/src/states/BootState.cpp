#include "states/BootState.h"
#include "GameController.h"

/** @section Ciclo de Vida */

void BootState::enter(GameController* controller)
{
    // Centraliza todos os servos para a posição neutra de segurança
    controller->getMotion().centerAll();
}

void BootState::exit(GameController* controller) {}

/** @section Atualização Lógica */

void BootState::update(GameController* controller)
{
    // Aguarda um tempo fixo de 5 segundos para garantir que o hardware
    // estabilizou
    if (millis() - controller->getStateStartTime() > 5000)
    {
        // Transiciona automaticamente para o estado de espera ativa (IDLE)
        controller->changeState(RobotState::IDLE);
    }
}

/** @section Tratamento de Eventos */

void BootState::handleRFID(GameController* controller, const String& uid) {}
