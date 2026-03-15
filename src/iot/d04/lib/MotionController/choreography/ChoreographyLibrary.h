#pragma once

#include "Choreography.h"
#include <vector>

/**
 * @class ChoreographyLibrary
 * @brief Coleção de coreografias de movimento pré-definidas.
 */
class ChoreographyLibrary
{
    public:
        // --- CABEÇA ---

        static std::vector<ChoreoStep> shakeHeadError()
        {
            return {
                {80, 0.1f, 30.0f},
                {100, 0.1f, 30.0f},
                {80, 0.1f, 30.0f},
                {100, 0.1f, 30.0f},
                {90, 0.5f, 20.0f}};
        }

        static std::vector<ChoreoStep> idlePanLook()
        {
            return {
                {60, 1.5f, 2.0f},
                {90, 0.5f, 2.0f},
                {120, 1.5f, 2.0f},
                {90, 1.0f, 2.0f}};
        }

        static std::vector<ChoreoStep> lookSide(int angle)
        {
            return {{angle, 1.0f, 10.0f}, {90, 0.5f, 10.0f}};
        }

        // --- BRAÇOS ---

        static std::vector<ChoreoStep> waveHand()
        {
            return {
                {140, 0.4f, 15.0f},
                {80, 0.4f, 15.0f},
                {140, 0.4f, 15.0f},
                {80, 0.4f, 15.0f},
                {0, 1.0f, 10.0f}};
        }

        static std::vector<ChoreoStep> askingForHelp()
        {
            return {{40, 1.5f, 5.0f}, {0, 1.0f, 5.0f}};
        }

        static std::vector<ChoreoStep> scrubL()
        {
            return {{160, 0.5f, 20.0f}, {20, 0.5f, 20.0f}};
        }

        static std::vector<ChoreoStep> scrubR()
        {
            return {{20, 0.5f, 20.0f}, {160, 0.5f, 20.0f}};
        }
};
