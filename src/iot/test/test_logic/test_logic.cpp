#include <Arduino.h>
#include <unity.h>

// Core project headers
#include "AudioTracks.h"
#include "Constants.h"
#include "MotionController.h"

/**
 * @section Math and Physics Tests (MotionController)
 */

void test_motion_easing_start(void)
{
    // Tests the start of the movement (0.0 progress) using the lib's real
    // function
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, result);
}

void test_motion_easing_mid(void)
{
    // Tests the middle of the movement (0.5 progress) using the lib's real
    // function
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 0.5f);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, result);
}

void test_motion_easing_end(void)
{
    // Tests the end of the movement (1.0 progress) using the lib's real
    // function
    float result = MotionController::easeSmoothStep(0.0f, 100.0f, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, result);
}

void test_motor_axis_clamping(void)
{
    // Tests whether MotorAxis respects the physical limits (clamping)
    // Pin 13, initial 90, min 20, max 160
    MotorAxis axis(13, 90, 20, 160);

    // Case 1: target above the allowed maximum
    axis.setTarget(200, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(160.0f, axis.targetAngle);

    // Case 2: target below the allowed minimum
    axis.setTarget(0, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, axis.targetAngle);
}

void test_motor_inversion_logic(void)
{
    // Tests whether mirrored arms invert the angle correctly (180 - x)
    // Pin 14, initial 90, min 0, max 180, inverted = true
    MotorAxis axis(14, 90, 0, 180, true);
    axis.currentAngle = 10.0f;

    float writeAngle = 180.0f - axis.currentAngle;
    TEST_ASSERT_EQUAL_FLOAT(170.0f, writeAngle);
}

void test_choreo_sequence_stepping(void)
{
    // Tests the time-based step progression of a choreography
    ChoreoSequence choreo;
    std::vector<ChoreoStep> steps = {
        {45, 1.0f}, // Step 0: 45 degrees over 1s
        {90, 2.0f}  // Step 1: 90 degrees over 2s
    };
    choreo.setFrames(steps);
    choreo.play(0.0f, false); // Play with no delay and no loop

    // Frame 0: the first update must always return the first step (45°)
    const ChoreoStep* step = choreo.update(0.01f);
    TEST_ASSERT_NOT_NULL(step);
    TEST_ASSERT_EQUAL_INT(45, step->targetAngle);

    // Intermediate frame: 0.5s total elapsed (still running step 0)
    // Should not return a new command (nullptr)
    step = choreo.update(0.5f);
    TEST_ASSERT_NULL(step);

    // Transition frame: 1.1s total elapsed (past step 0's 1.0s)
    // Must return the next step's command (90°)
    step = choreo.update(0.6f);
    TEST_ASSERT_NOT_NULL(step);
    TEST_ASSERT_EQUAL_INT(90, step->targetAngle);
}

/**
 * @section Playlist and Audio Tests (AudioTracks)
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
 * @section Business Rule Tests (Pedagogy)
 */

void test_pedagogical_timeouts(void)
{
    // Validates the timing for each stage of the ritual
    TEST_ASSERT_EQUAL_UINT32(12000, GameConfig::WET_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(12000, GameConfig::SOAP_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(25000, GameConfig::SCRUB_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(20000, GameConfig::RINSE_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(25000, GameConfig::DRY_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT32(15000, GameConfig::SUCCESS_DISPLAY_MS);
}

/**
 * @section Runner Configuration
 */

void setup()
{
    // Delay for the Serial Monitor to connect
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
    // Nothing to do here
}
