#pragma once

#include "AudioTypes.h"

// Macro para facilitar a criação de playlists calculando a duração
template <size_t N>
inline AudioPlaylist makePlaylist(const AudioTrack (&array)[N])
{
    unsigned long total = 0;
    for (size_t i = 0; i < N; ++i)
    {
        total += array[i].durationMs;
    }
    return {array, (uint8_t)N, total};
}

/**
 * @namespace AudioFiles
 * @brief Faixas de áudio individuais com ID e duração.
 */
namespace AudioFiles
{
    const AudioTrack BOOT = {1, 3632};
    const AudioTrack IDLE_READY = {2, 2915};
    const AudioTrack IDLE_REMINDER = {3, 2631};

    const AudioTrack WET_1 = {4, 4896};
    const AudioTrack WET_2 = {5, 1995};

    const AudioTrack SOAP_1 = {6, 1345};
    const AudioTrack SOAP_2 = {7, 1874};

    const AudioTrack SCRUB_1 = {8, 2407};
    const AudioTrack SCRUB_2 = {9, 2159};
    const AudioTrack SCRUB_3 = {10, 2952};

    const AudioTrack RINSE_1 = {11, 1692};
    const AudioTrack RINSE_2 = {12, 6749};
    const AudioTrack RINSE_3 = {13, 1831};

    const AudioTrack DRY_1 = {14, 2106};
    const AudioTrack DRY_2 = {15, 2688};
    const AudioTrack DRY_3 = {16, 2981};

    const AudioTrack WAITING_START = {17, 990};
    const AudioTrack WAITING_15S = {18, 1919};
    const AudioTrack WAITING_OFF = {19, 2922};

    const AudioTrack ERROR = {20, 2186};
    const AudioTrack SUCCESS = {21, 2097};
    const AudioTrack HIDE_AND_BEAK = {22, 8540};
} // namespace AudioFiles

/**
 * @namespace Playlists
 * @brief Sequências de áudio agrupadas por etapa do ritual.
 */
namespace Playlists
{
    const AudioTrack WET_TRACKS[] = {AudioFiles::WET_1, AudioFiles::WET_2};
    const AudioPlaylist WET = makePlaylist(WET_TRACKS);

    const AudioTrack SOAP_TRACKS[] = {AudioFiles::SOAP_1, AudioFiles::SOAP_2};
    const AudioPlaylist SOAP = makePlaylist(SOAP_TRACKS);

    const AudioTrack SCRUB_TRACKS[] = {
        AudioFiles::SCRUB_1,
        AudioFiles::SCRUB_2,
        AudioFiles::SCRUB_3};
    const AudioPlaylist SCRUB = makePlaylist(SCRUB_TRACKS);

    const AudioTrack RINSE_TRACKS[] = {
        AudioFiles::RINSE_1,
        AudioFiles::RINSE_2,
        AudioFiles::RINSE_3};
    const AudioPlaylist RINSE = makePlaylist(RINSE_TRACKS);

    const AudioTrack DRY_TRACKS[] = {
        AudioFiles::DRY_1,
        AudioFiles::DRY_2,
        AudioFiles::DRY_3};
    const AudioPlaylist DRY = makePlaylist(DRY_TRACKS);
} // namespace Playlists
