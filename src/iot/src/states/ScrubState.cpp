#include "states/ScrubState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> SCRUB_POOL = {BehaviorVignette(
    eEmotions::Glee,
    0.0f,
    -0.5f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::scrubArm(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::scrubArm(), 0.4f, true),
    GameConfig::SCRUB_TIMEOUT
)};

/** @section Ciclo de Vida */

void ScrubState::enter(GameController* controller)
{
    // Lógica Pedagógica: Calcula quantas vezes a playlist cabe no tempo da
    // etapa.
    uint8_t maxLoops =
        GameConfig::SCRUB_TIMEOUT / Playlists::SCRUB.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Feedback sonoro sequencial de SCRUB (Esfregar)
    controller->getAudio().playSequence(Playlists::SCRUB, maxLoops);

    // Efeito visual de higiene
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Executa movimento repetitivo de esfregação sincronizada entre os braços.
    controller->getBehaviors().setPool(SCRUB_POOL, 0, 0, false);
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
