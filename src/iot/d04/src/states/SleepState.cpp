#include "states/SleepState.h"
#include "GameController.h"

void SleepState::enter(GameController* controller)
{
    // Feedback físico: Centraliza motores antes de "dormir" para evitar tensões
    controller->getMotion().centerAll();

    // Nota: O desligamento real do hardware (Deep Sleep) ou display
    // deve ser orquestrado pelo PowerController se implementado.
}

void SleepState::update(GameController* controller)
{
    // Permanece em sono até um evento externo de hardware (botão/sensor)
    // Se o RFID acordar o sistema, ele deve ser tratado via interrupção
}

void SleepState::exit(GameController* controller) {}

void SleepState::handleRFID(GameController* controller, const String& uid) {}
