#pragma once

#include "Choreography.h"
#include "Common.h"
#include "FaceEmotions.hpp"
#include <Arduino.h>
#include <vector>

class DisplayOrchestrator;
class MotionController;
class AudioController;

#include "AudioTypes.h"

/**
 * @struct ChoreoAction
 * @brief Agrupa uma sequência de passos e um delay inicial para um motor.
 *
 * Utilizado pelo motor de comportamento para agendar movimentos específicos
 * em eixos individuais durante a execução de uma vinheta.
 */
struct ChoreoAction
{
        std::vector<ChoreoStep> steps; ///< Passos da coreografia.
        float delay = 0.0f;            ///< Atraso inicial em segundos.
        bool loop = false; ///< Se a coreografia deve repetir em loop.

        /**
         * @brief Construtor para facilitar a criação de ações com delay.
         * @param s Vetor de passos (frames) da coreografia.
         * @param d Tempo de espera em segundos antes de iniciar o movimento.
         * @param l Define se o movimento deve ser repetido continuamente.
         */
        ChoreoAction(
            std::vector<ChoreoStep> s = {},
            float d = 0.0f,
            bool l = false
        )
            : steps(s), delay(d), loop(l)
        {
        }
};

/**
 * @struct BehaviorVignette
 * @brief Representa uma "esquete" ou micro-comportamento atômico do robô.
 *
 * Define um estado completo de expressão (olhos, olhar, motores e som)
 * que o robô assume por um período determinado de tempo.
 */
struct BehaviorVignette
{
        eEmotions mood; ///< Humor/Expressão facial a ser aplicada.
        float lookX;    ///< Direção horizontal do olhar (-1.0 esquerda, 1.0
                        ///< direita).
        float lookY;    ///< Direção vertical do olhar (-1.0 baixo, 1.0 cima).
        ChoreoAction head; ///< Coreografia da cabeça.
        ChoreoAction armL; ///< Coreografia do braço esquerdo.
        ChoreoAction armR; ///< Coreografia do braço direito.
        AudioTrack
            audioTrack; ///< Áudio opcional a ser disparado (id 0 = nenhum).
        unsigned long durationMs; ///< Tempo de exibição desta vinheta (ms).

        /**
         * @brief Construtor completo para orquestração de múltiplos eixos e
         * áudio.
         * @param m Emoção/Humor visual.
         * @param lx Coordenada X do olhar.
         * @param ly Coordenada Y do olhar.
         * @param h Configuração de movimento da cabeça.
         * @param al Configuração de movimento do braço esquerdo.
         * @param ar Configuração de movimento do braço direito.
         * @param d Duração total da cena em milissegundos.
         * @param audio Referência da faixa de áudio a ser tocada.
         */
        BehaviorVignette(
            eEmotions m,
            float lx,
            float ly,
            ChoreoAction h = ChoreoAction(),
            ChoreoAction al = ChoreoAction(),
            ChoreoAction ar = ChoreoAction(),
            unsigned long d = 1000,
            AudioTrack audio = {0, 0}
        )
            : mood(m), lookX(lx), lookY(ly), head(h), armL(al), armR(ar),
              audioTrack(audio), durationMs(d)
        {
        }
};

/**
 * @class BehaviorEngine
 * @brief Motor de orquestração de comportamentos expressivos e motores.
 *
 * Esta classe gerencia a execução de "vinhetas" (BehaviorVignette), que são
 * combinações sincronizadas de expressões faciais, movimentos de braços/cabeça
 * e efeitos sonoros. O motor funciona de forma não-bloqueante, alternando entre
 * períodos de ação e repouso (silêncio orgânico).
 */
class BehaviorEngine
{
    public:
        /**
         * @brief Construtor padrão.
         */
        BehaviorEngine();

        /**
         * @brief Define o conjunto de comportamentos disponíveis e os
         * parâmetros de cadência.
         *
         * @param pool Vetor de vinhetas que o robô pode executar.
         * @param minPauseMs Tempo mínimo de repouso entre ações (ms).
         * @param maxPauseMs Tempo máximo de repouso entre ações (ms).
         * @param loop Se true, reinicia a lista ao chegar no fim.
         * @param resetOnRest Se true, reseta a pose e expressão durante o
         * repouso.
         */
        void setPool(
            const std::vector<BehaviorVignette>& pool,
            unsigned long minPauseMs = 1000,
            unsigned long maxPauseMs = 3000,
            bool loop = true,
            bool resetOnRest = false
        );

        /**
         * @brief Atualiza a lógica do motor, gerenciando timers e disparando
         * ações.
         *
         * Deve ser chamado continuamente no loop principal ou em task dedicada.
         *
         * @param display Referência ao orquestrador visual.
         * @param motion Referência ao controlador de movimento.
         * @param audio Referência ao controlador de áudio para disparar sons de
         * vinheta.
         */
        void update(
            DisplayOrchestrator& display,
            MotionController& motion,
            AudioController& audio
        );

        /**
         * @brief Interrompe qualquer ação em curso e limpa o pool.
         */
        void stop();

        /**
         * @brief Pausa ou retoma a execução do motor sem limpar o estado.
         * @param paused True para pausar, False para retomar.
         */
        void setPaused(bool paused) { _isPaused = paused; }

        /**
         * @brief Verifica se o robô está executando uma ação ativa no momento.
         * @return True se estiver em ação, False se estiver em repouso.
         */
        bool isInAction() const { return _isInAction; }

    private:
        std::vector<BehaviorVignette> _pool; ///< Lista de comportamentos ativos
        int _currentIndex = -1;              ///< Índice da vinheta atual
        unsigned long _nextEventTime = 0; ///< Timestamp para o próximo evento
        bool _isPaused = false;           ///< Estado de pausa do motor
        bool _isInAction = false;         ///< Se há uma ação sendo executada
        bool _loop = true;                ///< Se deve repetir o pool
        bool _resetOnRest =
            false; ///< Se deve voltar ao neutro durante o repouso

        unsigned long _minPause = 1000; ///< Pausa mínima configurada
        unsigned long _maxPause = 3000; ///< Pausa máxima configurada

        /**
         * @brief Seleciona e dispara a próxima ação do pool.
         * @param display Referência ao orquestrador visual.
         * @param motion Referência ao controlador de movimento.
         * @param audio Referência ao controlador de áudio.
         */
        void pickNextAction(
            DisplayOrchestrator& display,
            MotionController& motion,
            AudioController& audio
        );

        /**
         * @brief Finaliza a ação atual e entra no estado de repouso orgânico.
         * @param display Referência ao orquestrador visual.
         * @param motion Referência ao controlador de movimento.
         */
        void enterRest(DisplayOrchestrator& display, MotionController& motion);
};
