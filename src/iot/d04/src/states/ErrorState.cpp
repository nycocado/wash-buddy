#include "states/ErrorState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

/** @section Ciclo de Vida */

void ErrorState::enter(GameController* controller)
{
    // Feedback visual: Humor cético e animação de "hã?" no display
    controller->getDisplay().setEyeMood(eEmotions::Skeptic);
    controller->getDisplay().playConfused();

    // Feedback físico: Nega com a cabeça (shake head)
    controller->getMotion().playHeadChoreography(
        ChoreographyLibrary::shakeHeadError(), 0.0f, false
    );

    // Feedback físico: Levanta os braços em sinal de "não entendi"
    controller->getMotion().moveArmL(120, 15.0f);
    controller->getMotion().moveArmR(120, 15.0f);
}

void ErrorState::exit(GameController* controller) {}

/** @section Atualização Lógica */

void ErrorState::update(GameController* controller)
{
    // Após exibir o erro, decide para onde retornar
    if (millis() - controller->getStateStartTime() >
        GameConfig::ERROR_DISPLAY_MS)
    {
        State* prev = controller->getPreviousState();

        // Se já tinha começado o ritual, volta para WAITING (espera o item
        // correto)
        if (prev != nullptr && prev->getStateEnum() != RobotState::IDLE &&
            prev->getStateEnum() != RobotState::BOOT)
        {
            controller->changeState(RobotState::WAITING);
        }
        else
        {
            // Se errou logo no início, volta para o estado ocioso
            controller->changeState(RobotState::IDLE);
        }
    }
}

/** @section Tratamento de Eventos */

void ErrorState::handleRFID(GameController* controller, const String& uid) {}
