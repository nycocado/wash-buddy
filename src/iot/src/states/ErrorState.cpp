#include "states/ErrorState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

static const std::vector<BehaviorVignette> ERROR_POOL = {BehaviorVignette(
    eEmotions::Annoyed,
    0.0f,
    0.0f,
    ChoreoAction(ChoreographyLibrary::shakeHeadError()),
    ChoreoAction({{120, GameConfig::ERROR_DISPLAY_MS / 1000.0f}}),
    ChoreoAction({{120, GameConfig::ERROR_DISPLAY_MS / 1000.0f}}),
    GameConfig::ERROR_DISPLAY_MS
)};

/** @section Ciclo de Vida */

void ErrorState::enter(GameController* controller)
{
    // Feedback sonoro de Erro (NO)
    controller->getAudio().playFile(AudioFiles::ERROR);

    // Expressão facial de dúvida/confusão
    controller->getDisplay().playConfused();

    // Executa comportamento customizado via BehaviorEngine em loop único.
    controller->getBehaviors().setPool(ERROR_POOL, 0, 0, false);
}

void ErrorState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void ErrorState::update(GameController* controller)
{
    // Após exibir o erro pelo tempo configurado, decide para onde retornar.
    if (millis() - controller->getStateStartTime() >
        GameConfig::ERROR_DISPLAY_MS)
    {
        State* prev = controller->getPreviousState();

        // Se o erro ocorreu durante o ritual, volta para WAITING para que a
        // criança possa tentar novamente a etapa correta.
        if (prev != nullptr && prev->getStateEnum() != RobotState::IDLE &&
            prev->getStateEnum() != RobotState::BOOT)
        {
            controller->changeState(RobotState::WAITING);
        }
        else
        {
            // Se errou logo no início (Idle), volta para o repouso.
            controller->changeState(RobotState::IDLE);
        }
    }
}

/** @section Tratamento de Eventos */

void ErrorState::handleRFID(GameController* controller, const String& uid) {}
