#include "states/BootState.h"
#include "GameController.h"

void BootState::enter(GameController* controller)
{
    // Centraliza todos os servos para a posição neutra de segurança
    controller->getMotion().centerAll();
}

void BootState::update(GameController* controller)
{
    // Aguarda um tempo fixo de 3 segundos para garantir que o hardware
    // estabilizou
    if (millis() - controller->getStateStartTime() > 3000)
    {
        // Transiciona automaticamente para o estado de espera ativa (IDLE)
        controller->changeState(RobotState::IDLE);
    }
}

void BootState::exit(GameController* controller) {}

void BootState::handleRFID(GameController* controller, const String& uid) {}
