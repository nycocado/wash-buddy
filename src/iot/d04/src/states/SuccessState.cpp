#include "states/SuccessState.h"
#include "GameController.h"

/** @section Ciclo de Vida */

void SuccessState::enter(GameController* controller)
{
    // Feedback visual: Humor "Glee" e chacoalhar de alegria no display
    controller->getDisplay().setEyeMood(eEmotions::Glee);
    controller->getDisplay().playHappy();

    // Efeito festivo de confetes
    controller->getDisplay().setParticleEffect(EffectType::CONFETTI);

    // Feedback físico: Movimenta os braços em sinal de tchau/comemoração
    controller->getMotion().waveHands();
}

void SuccessState::exit(GameController* controller) {}

/** @section Atualização Lógica */

void SuccessState::update(GameController* controller)
{
    // Após o tempo de exibição do sucesso, retorna para IDLE para nova
    // interação
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::IDLE);
    }
}

/** @section Tratamento de Eventos */

void SuccessState::handleRFID(GameController* controller, const String& uid) {}
