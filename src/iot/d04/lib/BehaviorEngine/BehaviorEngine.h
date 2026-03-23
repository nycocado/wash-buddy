#pragma once

#include "Choreography.h"
#include "Common.h"
#include "FaceEmotions.hpp"
#include <Arduino.h>
#include <vector>

class DisplayOrchestrator;
class MotionController;

/**
 * @struct BehaviorVignette
 * @brief Representa uma "esquete" ou micro-comportamento atômico do robô.
 *
 * Uma vinheta combina uma expressão facial (mood), uma direção do olhar (look)
 * e, opcionalmente, uma sequência de movimentos físicos (choreography).
 */
struct BehaviorVignette
{
        eEmotions mood; ///< Humor/Expressão facial a ser aplicada.
        float lookX;    ///< Direção horizontal do olhar (-1.0 esquerda, 1.0
                        ///< direita).
        float lookY;    ///< Direção vertical do olhar (-1.0 baixo, 1.0 cima).
        std::vector<ChoreoStep>
            choreography;         ///< Lista de passos de movimento físico.
        unsigned long durationMs; ///< Tempo de exibição desta vinheta (ms).
};

/**
 * @class BehaviorEngine
 * @brief Motor de orquestração de comportamentos expressivos e motores.
 *
 * O BehaviorEngine gerencia "pools" de vinhetas de comportamento, permitindo
 * que o robô execute ações aleatórias de forma cíclica (Loop) ou única
 * (One-Shot). Ele introduz períodos de "Rest" (repouso) entre as ações para
 * simular um comportamento mais orgânico e menos hiperativo.
 */
class BehaviorEngine
{
    public:
        /**
         * @brief Construtor padrão do motor de comportamento.
         */
        BehaviorEngine();

        /**
         * @brief Define o conjunto de comportamentos (pool) a ser executado.
         *
         * @param pool Vetor contendo as vinhetas de comportamento.
         * @param minPauseMs Tempo mínimo de repouso entre vinhetas (ms).
         * @param maxPauseMs Tempo máximo de repouso entre vinhetas (ms).
         * @param loop Se false, executa apenas UMA vinheta aleatória e para.
         */
        void setPool(
            const std::vector<BehaviorVignette>& pool,
            unsigned long minPauseMs = 1000,
            unsigned long maxPauseMs = 3000,
            bool loop = true
        );

        /**
         * @brief Atualiza a lógica do motor, gerenciando timers e trocas de
         * vinheta. Deve ser chamado continuamente no loop principal.
         *
         * @param display Referência ao orquestrador visual.
         * @param motion Referência ao controlador de movimento.
         */
        void update(DisplayOrchestrator& display, MotionController& motion);

        /**
         * @brief Interrompe a execução atual e limpa o pool de comportamentos.
         */
        void stop();

        /**
         * @brief Pausa ou retoma a execução do motor sem limpar o pool.
         * @param paused True para pausar, False para retomar.
         */
        void setPaused(bool paused) { _isPaused = paused; }

        /**
         * @brief Verifica se uma ação (vignette) está em execução no momento.
         * @return True se estiver em ação, False se estiver em repouso ou
         * parado.
         */
        bool isInAction() const { return _isInAction; }

    private:
        std::vector<BehaviorVignette> _pool;
        int _currentIndex = -1;
        unsigned long _nextEventTime = 0;
        bool _isPaused = false;
        bool _isInAction = false;
        bool _loop = true;

        unsigned long _minPause = 1000;
        unsigned long _maxPause = 3000;

        /**
         * @brief Sorteia e inicia a próxima vinheta do pool.
         */
        void
        pickNextAction(DisplayOrchestrator& display, MotionController& motion);

        /**
         * @brief Finaliza a ação atual e entra em estado de repouso
         * (Normal/Centro).
         */
        void enterRest(DisplayOrchestrator& display);
};
