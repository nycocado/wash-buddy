#include "states/SoapState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

static const std::vector<BehaviorVignette> SOAP_POOL = {BehaviorVignette(
    eEmotions::Focused,
    0.0f,
    1.0f,
    ChoreoAction(),
    ChoreoAction(ChoreographyLibrary::soapSlide(), 0.0f, true),
    ChoreoAction(ChoreographyLibrary::soapSlide(), 0.8f, true), // Alternado
                                                                // perfeitamente
    GameConfig::SOAP_TIMEOUT
)};

/** @section Ciclo de Vida */

void SoapState::enter(GameController* controller)
{
    // Lógica Pedagógica: Calcula quantas vezes a playlist cabe no tempo da
    // etapa.
    uint8_t maxLoops =
        GameConfig::SOAP_TIMEOUT / Playlists::SOAP.totalDurationMs;
    if (maxLoops == 0)
        maxLoops = 1;

    // Feedback sonoro sequencial de SOAP (Bolhas)
    controller->getAudio().playSequence(Playlists::SOAP, maxLoops);

    // Efeito visual de higiene
    controller->getDisplay().setParticleEffect(EffectType::BUBBLES);

    // Executa movimento de 'pegar sabão' alternando os braços.
    controller->getBehaviors().setPool(SOAP_POOL, 0, 0, false);
}

void SoapState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void SoapState::update(GameController* controller)
{
    if (millis() - controller->getStateStartTime() > getTimeout())
    {
        controller->changeState(RobotState::WAITING);
    }
}

/** @section Tratamento de Eventos */

void SoapState::handleRFID(GameController* controller, const String& uid)
{
    validateRFID(
        controller, uid, RFIDTags::SOAP, RFIDTags::SCRUB, RobotState::SCRUB
    );
}
