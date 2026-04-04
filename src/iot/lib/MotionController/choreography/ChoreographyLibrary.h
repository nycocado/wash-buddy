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
                {80, 0.25f},
                {100, 0.25f},
                {80, 0.25f},
                {100, 0.25f},
                {90, 0.5f}};
        }

        static std::vector<ChoreoStep> idlePanLook()
        {
            return {{70, 2.0f}, {90, 0.8f}, {110, 2.0f}, {90, 1.5f}};
        }

        static std::vector<ChoreoStep> lookSide(int angle)
        {
            return {{angle, 1.2f}, {90, 0.8f}};
        }

        // --- BRAÇOS ---

        static std::vector<ChoreoStep> waveHand()
        {
            return {{10, 0.5f}, {50, 0.5f}, {10, 0.5f}, {90, 0.5f}};
        }

        static std::vector<ChoreoStep> celebrateVictory()
        {
            return {
                {110, 0.2f}, {70, 0.2f}, {110, 0.2f}, {70, 0.2f}, {90, 0.4f}};
        }

        static std::vector<ChoreoStep> scrubArm()
        {
            return {{0, 0.4f}, {40, 0.4f}};
        }

        static std::vector<ChoreoStep> wetDive()
        {
            return {
                {20, 1.0f},
                {25, 0.2f},
                {15, 0.2f},
                {25, 0.2f},
                {15, 0.2f},
                {20, 0.5f}};
        }

        static std::vector<ChoreoStep> soapSlide()
        {
            return {{30, 0.8f}, {120, 0.8f}, {30, 0.8f}, {90, 0.5f}};
        }

        static std::vector<ChoreoStep> dryFanHead()
        {
            return {
                {120, 0.2f},
                {140, 0.2f},
                {125, 0.1f},
                {135, 0.1f},
                {130, 0.3f}};
        }
};
