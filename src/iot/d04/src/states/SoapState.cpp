#include "states/SoapState.h"
#include "GameController.h"

void SoapState::enter(GameController* controller)
{
    // Feedback visual: Olhar focado e para cima (direção das mãos)
    controller->getDisplay().setEyeMood(eEmotions::Focused);
    controller->getDisplay().lookAt(0.0f, 1.0f);

    // Ativa o sistema de partículas com efeito de bolhas
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Prepara os motores para o próximo passo (esfregar)
    controller->getMotion().centerAll();
}

void SoapState::update(GameController* controller)
{
    // Retorna para WAITING se o tempo limite for atingido sem interação
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

void SoapState::exit(GameController* controller) {}

void SoapState::handleRFID(GameController* controller, const String& uid)
{
    // Transição lógica: Sabão -> Esfregar
    // Valida se o item aproximado é a buchinha/esponja
    validateRFID(
        controller, uid, RFIDTags::SOAP, RFIDTags::SCRUB, RobotState::SCRUB
    );
}
