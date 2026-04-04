#pragma once

#include <Arduino.h>

/**
 * @brief Estrutura que representa uma faixa de áudio única.
 */
struct AudioTrack
{
        uint16_t id;
        unsigned long durationMs;
};

/**
 * @brief Estrutura que representa uma playlist (sequência de faixas).
 */
struct AudioPlaylist
{
        const AudioTrack* tracks;
        uint8_t count;
        unsigned long totalDurationMs;
};
