#include "states/WetState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> WET_POOL = {
    {eEmotions::Surprised, 0.0f, -1.0f, std::vector<ChoreoStep>(), 3000},
    {eEmotions::Happy, 0.0f, -0.8f, std::vector<ChoreoStep>(), 3000},
    {eEmotions::Focused, 0.0f, -1.0f, ChoreographyLibrary::nodHead(), 2500}};

/** @section Ciclo de Vida */

void WetState::enter(GameController* controller)
{
    controller->getDisplay().setParticleEffect(EffectType::RAIN_LIGHT);

    // One-Shot: escolhe UMA e executa apenas UMA VEZ
    controller->getBehaviors().setPool(WET_POOL, 0, 0, false);

    controller->getMotion().moveArmL(70, 5.0f);
    controller->getMotion().moveArmR(70, 5.0f);
}

void WetState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void WetState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void WetState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::SOAP, RobotState::SOAP
    );
}
