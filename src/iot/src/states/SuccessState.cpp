#include "states/SuccessState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> SUCCESS_POOL = {BehaviorVignette(
    eEmotions::Happy,
    0.0f,
    0.0f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::celebrateVictory(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::celebrateVictory(), 0.1f, true),
    GameConfig::SUCCESS_DISPLAY_MS
)};

/** @section Ciclo de Vida */

void SuccessState::enter(GameController* controller)
{
    // Feedback sonoro de Sucesso (DAH-DAH WEEEE)
    controller->getAudio().playFile(AudioFiles::SUCCESS);

    // Feedback visual extra
    controller->getDisplay().playHappy();

    // Efeito festivo de confetes
    controller->getDisplay().setParticleEffect(EffectType::CONFETTI);

    // Executa comportamento de comemoração
    controller->getBehaviors().setPool(SUCCESS_POOL, 0, 0, false);
}

void SuccessState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void SuccessState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        // Após o tempo de sucesso, desliga o sistema enviando o sinal ao módulo
        controller->shutdownSystem();
    }
}

/** @section Tratamento de Eventos */

void SuccessState::handleRFID(GameController* controller, const String& uid) {}
