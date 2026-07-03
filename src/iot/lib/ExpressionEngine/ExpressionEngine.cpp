#include "ExpressionEngine.h"

// External reference to the global u8g2 library instance.
// The expression engine uses this instance to draw the basic geometric
// shapes that make up the eyes.
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

ExpressionEngine::ExpressionEngine() : _face(nullptr) {}

ExpressionEngine::~ExpressionEngine()
{
    if (_face)
    {
        delete _face;
    }
}

/**
 * @section Lifecycle and Rendering
 */

void ExpressionEngine::begin(
    uint16_t width,
    uint16_t height,
    const ExpressionConfig& config
)
{
    // The Face class initializes u8g2 internally and sets up the buffer.
    // The eye size defines the rendering scale on the display.
    _face = new Face(width, height, config.eyeSize);

    // --- INITIAL SETUP (behavior) ---
    // We enable random blinking but keep the gaze fixed initially.
    _face->RandomBlink = true;
    _face->Blink.Timer.SetIntervalMillis(config.blinkInterval);
    _face->RandomLook = true;
    _face->RandomBehavior = false;

    // Ensures the robot starts with a neutral expression
    _face->Expression.GoTo_Normal();

    // Clears any residual variations (partial blinks or twitches)
    _face->LeftEye.Variation1.Clear();
    _face->LeftEye.Variation2.Clear();
    _face->RightEye.Variation1.Clear();
    _face->RightEye.Variation2.Clear();
}

void ExpressionEngine::update()
{
    if (_face)
    {
        // --- BEHAVIOR PROCESSING ---
        // If random mode is active, rolls new emotions.
        if (_face->RandomBehavior)
            _face->Behavior.Update();

        // Processes the smooth gaze movement (LookAt or RandomLook).
        if (_face->RandomLook)
            _face->Look.Update();

        // Processes the automatic blink timer.
        if (_face->RandomBlink)
            _face->Blink.Update();
    }
}

void ExpressionEngine::draw()
{
    if (_face)
    {
        // --- POSITIONING CALCULATION ---
        // Computes the center of each eye based on the interpupillary
        // distance and the base size defined in the configuration.

        // Left eye
        _face->LeftEye.CenterX =
            _face->CenterX - _face->EyeSize / 2 - _face->EyeInterDistance;
        _face->LeftEye.CenterY = _face->CenterY;
        _face->LeftEye.Draw();

        // Right eye
        _face->RightEye.CenterX =
            _face->CenterX + _face->EyeSize / 2 + _face->EyeInterDistance;
        _face->RightEye.CenterY = _face->CenterY;
        _face->RightEye.Draw();
    }
}

/**
 * @section Expression and Behavior Management
 */

void ExpressionEngine::setMood(eEmotions mood)
{
    if (_face)
    {
        // Changes the current mood. The library will process the
        // transition smoothly on the next drawing cycle.
        _face->Behavior.GoToEmotion(mood);
    }
}

void ExpressionEngine::setIdleMode(bool active)
{
    if (_face)
    {
        // Activates behaviors that give the robot "life" when it isn't
        // performing a specific task.
        _face->RandomLook = active;
        _face->RandomBlink = active;
    }
}

/**
 * @section Directional Controls and Presets
 */

void ExpressionEngine::blink()
{
    if (_face)
        _face->DoBlink();
}

void ExpressionEngine::lookAt(float x, float y)
{
    if (_face)
    {
        // Disables random look to focus on the requested coordinate.
        _face->RandomLook = false;
        _face->Look.LookAt(x, y);
    }
}

void ExpressionEngine::playConfused()
{
    if (_face)
        _face->Expression.GoTo_Annoyed();
}

void ExpressionEngine::playHappy()
{
    if (_face)
        _face->Expression.GoTo_Happy();
}
