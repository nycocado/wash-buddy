#pragma once
#include "State.h"

/**
 * @brief Estado de celebração ao concluir o ritual corretamente.
 *
 * Ativado após o sucesso na fase de secagem.
 * O objetivo é recompensar o usuário e reforçar o hábito positivo.
 *
 * Feedback Visual: Expressão de extrema felicidade (Glee),
 * animação de vibração e partículas de confete.
 * Feedback Físico: Acena as mãos em comemoração.
 */
class SuccessState : public State
{
    public:
        /** @brief Ativa confetes, humor festivo e aceno de mãos. */
        void enter(GameController* controller) override;

        /** @brief Retorna para IDLE após exibição do sucesso. */
        void update(GameController* controller) override;

        /** @brief Sem ações de saída. */
        void exit(GameController* controller) override;

        /** @brief Ignora tags durante a celebração. */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::SUCCESS. */
        RobotState getStateEnum() const override { return RobotState::SUCCESS; }

        /** @brief Retorna o tempo configurado para comemoração. */
        unsigned long getTimeout() const override
        {
            return GameConfig::SUCCESS_DISPLAY;
        }
};
