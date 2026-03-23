#include "states/RinseState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> RINSE_POOL = {
    {eEmotions::Squint, 0.0f, -1.0f, std::vector<ChoreoStep>(), 2500},
    {eEmotions::Focused, 0.0f, -0.8f, std::vector<ChoreoStep>(), 2000},
    {eEmotions::Happy, 0.0f, -0.5f, std::vector<ChoreoStep>(), 3000}};

/** @section Ciclo de Vida */

void RinseState::enter(GameController* controller)
{
    controller->getDisplay().setParticleEffect(EffectType::RAIN_HEAVY);

    // One-Shot: escolhe UMA e executa apenas UMA VEZ
    controller->getBehaviors().setPool(RINSE_POOL, 0, 0, false);

    controller->getMotion().centerAll();
}

void RinseState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void RinseState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void RinseState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::TOWEL, RobotState::DRY
    );
}
