#include "states/ScrubState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> SCRUB_POOL = {
    {eEmotions::Happy, 0.0f, -0.5f, std::vector<ChoreoStep>(), 2000},
    {eEmotions::Focused, 0.0f, -0.8f, std::vector<ChoreoStep>(), 2500},
    {eEmotions::Glee, 0.0f, 0.0f, std::vector<ChoreoStep>(), 2000}};

/** @section Ciclo de Vida */

void ScrubState::enter(GameController* controller)
{
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // One-Shot: escolhe UMA e executa apenas UMA VEZ
    controller->getBehaviors().setPool(SCRUB_POOL, 0, 0, false);

    controller->getMotion().playArmLChoreography(ChoreographyLibrary::scrubL());
    controller->getMotion().playArmRChoreography(ChoreographyLibrary::scrubR());
}

void ScrubState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void ScrubState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void ScrubState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::SCRUB, RFIDTags::FAUCET, RobotState::RINSE
    );
}
