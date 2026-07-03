#pragma once

#include <Arduino.h>

/**
 * @brief Structure representing a single audio track.
 */
struct AudioTrack
{
        uint16_t id;
        unsigned long durationMs;
};

/**
 * @brief Structure representing a playlist (sequence of tracks).
 */
struct AudioPlaylist
{
        const AudioTrack* tracks;
        uint8_t count;
        unsigned long totalDurationMs;
};
