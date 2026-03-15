#pragma once
#include "State.h"

/**
 * @brief Representa a etapa de aplicação de sabão no ritual de lavagem.
 *
 * Este estado é ativado quando o usuário aproxima o sabonete (ou dispenser) do
 * robô. O objetivo é indicar que o robô reconheceu o início da limpeza química.
 *
 * Feedback Visual: Expressão focada, olhos voltados para cima (como se olhasse
 * para as mãos) e efeito de partículas de bolhas no display. Feedback Físico:
 * Os motores são centralizados para preparar para a esfregação.
 */
class SoapState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::SOAP; }
        unsigned long getTimeout() const override
        {
            return GameConfig::SOAP_TIMEOUT;
        }
};
