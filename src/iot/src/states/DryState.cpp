#include "states/DryState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> DRY_POOL = {
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900

    ),
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        0.1f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900
    ),
    BehaviorVignette(
        eEmotions::Sleepy,
        -0.7f,
        -0.1f,
        ChoreoAction(ChoreographyLibrary::dryFanHead(), 0.0f, true),
        ChoreoAction(),
        ChoreoAction(),
        900
    )};

/** @section Ciclo de Vida */

void DryState::enter(GameController* controller)
{
    // Lógica Pedagógica: Calcula quantas vezes a playlist cabe no tempo da
    // etapa para evitar silêncio ou cortes abruptos.
    uint8_t maxLoops = GameConfig::DRY_TIMEOUT / Playlists::DRY.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Feedback sonoro sequencial de DRY (Vento)
    controller->getAudio().playSequence(Playlists::DRY, maxLoops);

    // Efeito visual atmosférico
    controller->getDisplay().setParticleEffect(EffectType::WIND);

    // Executa a vibração de 'sopros' durante todo o ritual de secagem.
    controller->getBehaviors().setPool(DRY_POOL, 0, 0, true);
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
        // Ritual completo com sucesso.
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
