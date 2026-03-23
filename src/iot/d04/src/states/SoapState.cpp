#include "states/SoapState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> SOAP_POOL = {
    {eEmotions::Focused, 0.0f, 1.0f, std::vector<ChoreoStep>(), 3000},
    {eEmotions::Surprised, 0.3f, 0.8f, std::vector<ChoreoStep>(), 2000},
    {eEmotions::Happy, -0.3f, 0.8f, std::vector<ChoreoStep>(), 2000}};

/** @section Ciclo de Vida */

void SoapState::enter(GameController* controller)
{
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // One-Shot: escolhe UMA e executa apenas UMA VEZ
    controller->getBehaviors().setPool(SOAP_POOL, 0, 0, false);

    controller->getMotion().centerAll();
}

void SoapState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void SoapState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void SoapState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::SOAP, RFIDTags::SCRUB, RobotState::SCRUB
    );
}
