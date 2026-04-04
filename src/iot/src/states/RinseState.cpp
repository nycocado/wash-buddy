#include "states/RinseState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> RINSE_POOL = {BehaviorVignette(
    eEmotions::Happy,
    0.0f,
    -0.5f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.2f, true),
    GameConfig::RINSE_TIMEOUT
)};

/** @section Ciclo de Vida */

void RinseState::enter(GameController* controller)
{
    // Lógica Pedagógica: Calcula quantas vezes a playlist cabe no tempo da
    // etapa.
    uint8_t maxLoops =
        GameConfig::RINSE_TIMEOUT / Playlists::RINSE.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Feedback sonoro sequencial de RINSE (Enxágue)
    controller->getAudio().playSequence(Playlists::RINSE, maxLoops);

    // Efeito visual de chuva pesada
    controller->getDisplay().setParticleEffect(EffectType::RAIN_HEAVY);

    // Movimento de 'mergulho' simulando as mãos sob a água.
    controller->getBehaviors().setPool(RINSE_POOL, 0, 0, false);
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
