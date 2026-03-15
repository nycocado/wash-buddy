#include "states/RinseState.h"
#include "GameController.h"

void RinseState::enter(GameController* controller)
{
    // Feedback visual: "Squint" para simular os olhos sendo lavados
    controller->getDisplay().setEyeMood(eEmotions::Squint);
    controller->getDisplay().lookAt(0.0f, -1.0f); // Olha para as mãos na água

    // Efeito de partículas que simula o fluxo da torneira
    controller->getDisplay().setParticleEffect(EffectType::RAIN_HEAVY);

    // Estabiliza os motores
    controller->getMotion().centerAll();
}

void RinseState::update(GameController* controller)
{
    // Aguarda o próximo passo (Secar) ou retorna para espera
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

void RinseState::exit(GameController* controller) {}

void RinseState::handleRFID(GameController* controller, const String& uid)
{
    // Transição lógica: Enxaguar -> Secar
    // Espera que o usuário pegue a toalha (tag da toalha)
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::TOWEL, RobotState::DRY
    );
}
