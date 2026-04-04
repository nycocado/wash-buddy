#include "states/WetState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> WET_POOL = {BehaviorVignette(
    eEmotions::Focused,
    0.0f,
    -0.8f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::wetDive(), 0.2f, true),
    GameConfig::WET_TIMEOUT
)};

/** @section Ciclo de Vida */

void WetState::enter(GameController* controller)
{
    // Lógica Pedagógica: Calcula quantas vezes a playlist cabe no tempo da
    // etapa
    uint8_t maxLoops = GameConfig::WET_TIMEOUT / Playlists::WET.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Feedback sonoro sequencial de WET (Chuva)
    controller->getAudio().playSequence(Playlists::WET, maxLoops);

    controller->getDisplay().setParticleEffect(EffectType::RAIN_LIGHT);
    controller->getBehaviors().setPool(WET_POOL, 0, 0, false);
}

void WetState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void WetState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void WetState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::FAUCET, RFIDTags::SOAP, RobotState::SOAP
    );
}
