#include "states/SleepState.h"
#include "GameController.h"

/** @section Ciclo de Vida */

void SleepState::enter(GameController* controller)
{
    // Feedback físico: Centraliza motores antes de "dormir" para evitar tensões
    controller->getMotion().centerAll();

    // Nota: O desligamento real do hardware (Deep Sleep) ou display
    // deve ser orquestrado pelo PowerController se implementado.
}

void SleepState::exit(GameController* controller) {}

/** @section Atualização Lógica */

void SleepState::update(GameController* controller)
{
    // Permanece em sono até um evento externo de hardware (botão/sensor)
    // Se o RFID acordar o sistema, ele deve ser tratado via interrupção
}

/** @section Tratamento de Eventos */

void SleepState::handleRFID(GameController* controller, const String& uid) {}
