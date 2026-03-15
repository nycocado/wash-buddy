#include "states/WetState.h"
#include "GameController.h"

void WetState::enter(GameController* controller)
{
    controller->getDisplay().setEyeMood(eEmotions::Surprised);
    controller->getDisplay().setParticleEffect(EffectType::RAIN_LIGHT);
    controller->getDisplay().lookAt(0.0f, -1.0f);
    controller->getMotion().moveHead(90, 5.0f);
    controller->getMotion().moveArmL(40, 5.0f);
    controller->getMotion().moveArmR(40, 5.0f);
}

void WetState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

void WetState::exit(GameController* controller) {}

void WetState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::SOAP, RobotState::SOAP
    );
}
