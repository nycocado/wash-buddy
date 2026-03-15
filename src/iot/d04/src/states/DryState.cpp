#include "states/DryState.h"
#include "GameController.h"

void DryState::enter(GameController* controller)
{
    // Feedback visual: Admiração pelo progresso e efeito de vento (secagem)
    controller->getDisplay().setEyeMood(eEmotions::Awe);
    controller->getDisplay().setParticleEffect(EffectType::WIND);

    // Feedback físico: O robô olha para a esquerda onde a toalha deve estar
    controller->getDisplay().lookAt(1.0f, 0.0f);
    controller->getMotion().lookLeft();
}

void DryState::update(GameController* controller)
{
    // Se o usuário terminar a secagem (timeout ou RFID), vai para sucesso
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::SUCCESS);
    }
}

void DryState::exit(GameController* controller) {}

void DryState::handleRFID(GameController* controller, const String& uid)
{
    // Transição lógica: Secar -> Sucesso
    // Valida o uso da toalha para finalizar o ritual
    validateRFID(
        controller, uid, RFIDTags::TOWEL, nullptr, RobotState::SUCCESS
    );
}
