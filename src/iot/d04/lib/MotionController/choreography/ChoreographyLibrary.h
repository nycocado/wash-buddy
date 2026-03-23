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
                {80, 0.25f, 15.0f},
                {100, 0.25f, 15.0f},
                {80, 0.25f, 15.0f},
                {100, 0.25f, 15.0f},
                {90, 0.5f, 10.0f}};
        }

        static std::vector<ChoreoStep> idlePanLook()
        {
            return {
                {70, 2.0f, 1.5f},
                {90, 0.8f, 1.5f},
                {110, 2.0f, 1.5f},
                {90, 1.5f, 1.5f}};
        }

        static std::vector<ChoreoStep> lookSide(int angle)
        {
            return {{angle, 1.2f, 8.0f}, {90, 0.8f, 8.0f}};
        }

        static std::vector<ChoreoStep> nodHead()
        {
            return {{85, 0.2f, 20.0f}, {95, 0.2f, 20.0f}, {90, 0.4f, 15.0f}};
        }

        // --- BRAÇOS ---

        static std::vector<ChoreoStep> waveHand()
        {
            return {
                {10, 0.8f, 12.0f},
                {50, 0.8f, 12.0f},
                {10, 0.8f, 12.0f},
                {90, 0.8f, 12.0f}};
        }

        static std::vector<ChoreoStep> askingForHelp()
        {
            return {{120, 1.8f, 4.0f}, {90, 1.2f, 4.0f}};
        }

        static std::vector<ChoreoStep> scrubL()
        {
            return {{140, 0.9f, 8.0f}, {40, 0.9f, 8.0f}};
        }

        static std::vector<ChoreoStep> scrubR()
        {
            return {{40, 0.9f, 8.0f}, {140, 0.9f, 8.0f}};
        }
};
