#include "states/DryState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> DRY_POOL = {
    {eEmotions::Awe, -1.0f, 0.0f, std::vector<ChoreoStep>(), 3000},
    {eEmotions::Happy, 0.0f, 0.0f, std::vector<ChoreoStep>(), 2000},
    {eEmotions::Squint, -0.5f, 0.0f, std::vector<ChoreoStep>(), 2500}};

/** @section Ciclo de Vida */

void DryState::enter(GameController* controller)
{
    controller->getDisplay().setParticleEffect(EffectType::WIND);

    // One-Shot: escolhe UMA e executa apenas UMA VEZ
    controller->getBehaviors().setPool(DRY_POOL, 0, 0, false);
}

void DryState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void DryState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::SUCCESS);
    }
}

/** @section Tratamento de Eventos */

void DryState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::TOWEL, nullptr, RobotState::SUCCESS
    );
}
