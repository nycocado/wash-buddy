#pragma once
#include "State.h"

/**
 * @class WaitingState
 * @brief Estado de espera ativa por interações durante o ritual.
 *
 * Este estado é acionado quando uma etapa do ritual termina e o robô aguarda
 * que a criança aproxime a próxima tag RFID (ex: após molhar, espera o sabão).
 * Se a espera for muito longa (timeout), o robô assume que o ritual foi
 * abandonado e se desliga.
 *
 * O robô demonstra impaciência crescente através de expressões "preocupadas"
 * e depois "tristes", incentivando o retorno à atividade.
 */
class WaitingState : public State
{
    public:
        /**
         * @brief Inicia o estado de espera e exibe a instrução da próxima
         * etapa.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Gerencia a evolução do humor e o timeout de inatividade.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Finaliza os comportamentos de espera ao sair.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida se a tag lida corresponde ao próximo passo ou repetição
         * da etapa.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::WAITING.
         * @return RobotState O estado de espera.
         */
        RobotState getStateEnum() const override { return RobotState::WAITING; }

    private:
        bool _isWorriedPhase =
            false; ///< Indica se o robô começou a procurar o usuário.
        bool _isSadPhase =
            false; ///< Indica se o robô entrou em fase de decepção.

        const uint8_t* _nextIcon =
            nullptr; ///< Ícone da próxima etapa pedagógica.
        unsigned long _lastReminderTime =
            0; ///< Timestamp do último lembrete visual.
        bool _waitingAudioPlayed =
            false; ///< Se o áudio de início da espera já tocou.
        bool _sadAudioPlayed = false; ///< Se o áudio triste já tocou.
};
