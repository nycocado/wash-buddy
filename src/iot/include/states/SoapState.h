#pragma once
#include "State.h"

/**
 * @class SoapState
 * @brief Etapa de aplicação de sabão.
 *
 * Este estado é ativado quando a criança aproxima a tag do Sabonete (Soap).
 * Indica que o robô reconheceu o início da limpeza química.
 *
 * Feedback Visual: Expressão de foco e efeito de partículas de bolhas.
 * Feedback Físico: Os motores executam movimentos alternados simulando o ato
 * de receber sabão nas mãos.
 */
class SoapState : public State
{
    public:
        /**
         * @brief Ativa o efeito de bolhas e sonorização de aplicação de sabão.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitora o tempo de aplicação e transiciona para espera após
         * conclusão.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Limpa animações e efeitos de sabão.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida se o sabão continua sendo aplicado ou avança para
         * esfregação.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::SOAP.
         * @return RobotState O estado de aplicação de sabão.
         */
        RobotState getStateEnum() const override { return RobotState::SOAP; }

        /**
         * @brief Retorna o tempo pedagógico configurado para a aplicação de
         * sabão.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SOAP_TIMEOUT;
        }
};
