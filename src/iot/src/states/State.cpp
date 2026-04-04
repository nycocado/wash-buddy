#include "states/State.h"
#include "GameController.h"

/** @section Tratamento de Eventos */

void State::validateRFID(
    GameController* controller,
    const String& uid,
    const char* currentTag,
    const char* nextTag,
    RobotState nextState
)
{
    // CASO 1: A tag lida é a mesma que iniciou o estado atual.
    // Isso é interpretado como uma tentativa de repetir ou reiniciar a etapa
    // atual.
    if (uid == currentTag)
    {
        controller->handleRepeat();
    }
    // CASO 2: A tag lida é a tag esperada para a PRÓXIMA etapa do ritual.
    // Permite que o ritual avance para o próximo estado lógico.
    else if (nextTag != nullptr && uid == nextTag)
    {
        controller->changeState(nextState);
    }
    // CASO 3: Qualquer outra tag foi lida (ordem incorreta ou tag
    // desconhecida). Transiciona para o estado de erro, indicando que a criança
    // deve prestar atenção.
    else
    {
        controller->changeState(RobotState::ERROR);
    }
}
