#include "states/ScrubState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

void ScrubState::enter(GameController* controller)
{
    // Feedback visual: Humor feliz para reforçar o bom comportamento
    controller->getDisplay().setEyeMood(eEmotions::Happy);
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Feedback físico: Inicia a coreografia de esfregação nos dois braços
    controller->getMotion().playArmLChoreography(ChoreographyLibrary::scrubL());
    controller->getMotion().playArmRChoreography(ChoreographyLibrary::scrubR());
}

void ScrubState::update(GameController* controller)
{
    // Aguarda o próximo passo (Enxágue) ou retorna para espera
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

void ScrubState::exit(GameController* controller) {}

void ScrubState::handleRFID(GameController* controller, const String& uid)
{
    // Transição lógica: Esfregar -> Enxaguar
    // Espera que o usuário "abra a torneira" (aproximando o tag da torneira)
    validateRFID(
        controller, uid, RFIDTags::SCRUB, RFIDTags::FAUCET, RobotState::RINSE
    );
}
