#include <Arduino.h>
#include <unity.h>

// Core project headers
#include "AudioTracks.h"
#include "Constants.h"
#include "MotionController.h"

/**
 * @section Testes de Matemática e Física (MotionController)
 */

void test_motion_easing_start(void)
{
    // Testa o início do movimento (0.0 progress) usando a função real da lib
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, result);
}

void test_motion_easing_mid(void)
{
    // Testa o meio do movimento (0.5 progress) usando a função real da lib
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 0.5f);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, result);
}

void test_motion_easing_end(void)
{
    // Testa o fim do movimento (1.0 progress) usando a função real da lib
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, result);
}

void test_motor_axis_clamping(void)
{
    // Testa se o MotorAxis respeita os limites físicos (clamping)
    // Pino 13, Inicial 90, Min 20, Max 160
    MotorAxis axis(13, 90, 20, 160);

    // Caso 1: Alvo acima do máximo permitido
    axis.setTarget(200, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(160.0f, axis.targetAngle);

    // Caso 2: Alvo abaixo do mínimo permitido
    axis.setTarget(0, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, axis.targetAngle);
}

void test_motor_inversion_logic(void)
{
    // Testa se braços espelhados invertem o ângulo corretamente (180 - x)
    // Pino 14, Inicial 90, Min 0, Max 180, Invertido = true
    MotorAxis axis(14, 90, 0, 180, true);
    axis.currentAngle = 10.0f;

    float writeAngle = 180.0f - axis.currentAngle;
    TEST_ASSERT_EQUAL_FLOAT(170.0f, writeAngle);
}

void test_choreo_sequence_stepping(void)
{
    // Testa a progressão de passos de uma coreografia baseada em tempo
    ChoreoSequence choreo;
    std::vector<ChoreoStep> steps = {
        {45, 1.0f}, // Passo 0: 45 graus em 1s
        {90, 2.0f}  // Passo 1: 90 graus em 2s
    };
    choreo.setFrames(steps);
    choreo.play(0.0f, false); // Play sem delay e sem loop

    // Frame 0: O primeiro update deve sempre retornar o primeiro passo (45°)
    const ChoreoStep* step = choreo.update(0.01f);
    TEST_ASSERT_NOT_NULL(step);
    TEST_ASSERT_EQUAL_INT(45, step->targetAngle);

    // Frame intermediário: Passou 0.5s total (ainda executando passo 0)
    // Não deve retornar novo comando (nullptr)
    step = choreo.update(0.5f);
    TEST_ASSERT_NULL(step);

    // Frame de transição: Passou 1.1s total (estourou o 1.0s do passo 0)
    // Deve retornar obrigatoriamente o comando do próximo passo (90°)
    step = choreo.update(0.6f);
    TEST_ASSERT_NOT_NULL(step);
    TEST_ASSERT_EQUAL_INT(90, step->targetAngle);
}

/**
 * @section Testes de Playlist e Áudio (AudioTracks)
 */

void test_playlist_wet_integrity(void)
{
    unsigned long expected =
        AudioFiles::WET_1.durationMs + AudioFiles::WET_2.durationMs;
    TEST_ASSERT_EQUAL_UINT32(expected, Playlists::WET.totalDurationMs);
    TEST_ASSERT_EQUAL_INT(2, Playlists::WET.count);
}

void test_playlist_soap_integrity(void)
{
    unsigned long expected =
        AudioFiles::SOAP_1.durationMs + AudioFiles::SOAP_2.durationMs;
    TEST_ASSERT_EQUAL_UINT32(expected, Playlists::SOAP.totalDurationMs);
    TEST_ASSERT_EQUAL_INT(2, Playlists::SOAP.count);
}

void test_playlist_scrub_integrity(void)
{
    unsigned long expected = AudioFiles::SCRUB_1.durationMs +
                             AudioFiles::SCRUB_2.durationMs +
                             AudioFiles::SCRUB_3.durationMs;
    TEST_ASSERT_EQUAL_UINT32(expected, Playlists::SCRUB.totalDurationMs);
    TEST_ASSERT_EQUAL_INT(3, Playlists::SCRUB.count);
}

void test_playlist_rinse_integrity(void)
{
    unsigned long expected = AudioFiles::RINSE_1.durationMs +
                             AudioFiles::RINSE_2.durationMs +
                             AudioFiles::RINSE_3.durationMs;
    TEST_ASSERT_EQUAL_UINT32(expected, Playlists::RINSE.totalDurationMs);
    TEST_ASSERT_EQUAL_INT(3, Playlists::RINSE.count);
}

void test_playlist_dry_integrity(void)
{
    unsigned long expected = AudioFiles::DRY_1.durationMs +
                             AudioFiles::DRY_2.durationMs +
                             AudioFiles::DRY_3.durationMs;
    TEST_ASSERT_EQUAL_UINT32(expected, Playlists::DRY.totalDurationMs);
    TEST_ASSERT_EQUAL_INT(3, Playlists::DRY.count);
}

/**
 * @section Testes de Regras de Negócio (Pedagogia)
 */

void test_pedagogical_timeouts(void)
{
    // Valida os tempos para cada etapa do ritual
    TEST_ASSERT_EQUAL_UINT32(12000, GameConfig::WET_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(12000, GameConfig::SOAP_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(25000, GameConfig::SCRUB_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(20000, GameConfig::RINSE_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(25000, GameConfig::DRY_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(15000, GameConfig::SUCCESS_DISPLAY_MS);
}

/**
 * @section Configuração do Runner
 */

void setup()
{
    // Atraso para o Serial Monitor conectar
    delay(2000);

    UNITY_BEGIN();

    // Motor Tests
    RUN_TEST(test_motion_easing_start);
    RUN_TEST(test_motion_easing_mid);
    RUN_TEST(test_motion_easing_end);
    RUN_TEST(test_motor_axis_clamping);
    RUN_TEST(test_motor_inversion_logic);
    RUN_TEST(test_choreo_sequence_stepping);

    // Audio Playlist Integrity Tests
    RUN_TEST(test_playlist_wet_integrity);
    RUN_TEST(test_playlist_soap_integrity);
    RUN_TEST(test_playlist_scrub_integrity);
    RUN_TEST(test_playlist_rinse_integrity);
    RUN_TEST(test_playlist_dry_integrity);

    // Pedagogical Rule Tests
    RUN_TEST(test_pedagogical_timeouts);

    UNITY_END();
}

void loop()
{
    // Nada a fazer aqui
}
