#include "DisplayOrchestrator.h"

// External reference to the global u8g2 library instance.
// This instance is shared between the drawing engines to avoid memory
// conflicts and ensure atomic rendering.
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

DisplayOrchestrator::DisplayOrchestrator(const DisplayConfig& config)
    : _config(config), _eyes(), // Facial expression and behavior engine
      _animations(u8g2),        // Particle system bound to the u8g2 buffer
      _lastUpdate(0), _debugText("")
{
}

void DisplayOrchestrator::init()
{
    // Initializes the eye logic using the orchestrator's dimensions
    _eyes.begin(_config.screenWidth, _config.screenHeight);

    Serial.println(
        F("[DISPLAY] Orquestrador visual inicializado com sucesso.")
    );
}

void DisplayOrchestrator::update(float deltaTime)
{
    unsigned long now = millis();

    // --- PROGRESSIVE TRANSITION LOGIC (CURTAINS) ---
    // Manages the closing and opening of the "curtain" between the eyes
    // and icons
    const float transitionSpeed = 5.0f; // Transition speed

    if (_transitionState == TransitionState::CLOSING)
    {
        _transitionProgress += transitionSpeed * deltaTime;
        if (_transitionProgress >= 1.0f)
        {
            _transitionProgress = 1.0f;
            _transitionState = TransitionState::OPENING;
            _currentInstructionIcon = _pendingIcon;
            _instructionStartTime = now;
        }
    }
    else if (_transitionState == TransitionState::OPENING)
    {
        _transitionProgress -= transitionSpeed * deltaTime;
        if (_transitionProgress <= 0.0f)
        {
            _transitionProgress = 0.0f;
            _transitionState = TransitionState::IDLE;
        }
    }

    // --- ICON EXPIRATION LOGIC ---
    if (_currentInstructionIcon != nullptr && _instructionEndTime > 0 &&
        now > _instructionEndTime && _transitionState == TransitionState::IDLE)
    {
        // If the icon expires, starts the transition back to the eyes
        _pendingIcon = nullptr;
        _transitionState = TransitionState::CLOSING;
    }

    // 1. Clears the drawing buffer
    u8g2.clearBuffer();

    // 2. Draws the base layer (instruction icon OR eyes)
    if (_currentInstructionIcon != nullptr)
    {
        // Floating animation (only while not opening/closing)
        float floatOffset = 0;
        if (_transitionState == TransitionState::IDLE)
        {
            floatOffset = sin(now * 0.004f) * 3.0f;
        }

        int x = (128 - _instructionWidth) / 2;
        int y = (int)((64 - _instructionHeight) / 2 + floatOffset);
        u8g2.drawXBMP(
            x, y, _instructionWidth, _instructionHeight, _currentInstructionIcon
        );
    }
    else
    {
        _eyes.update();
        _eyes.draw();
    }

    // 3. Transition "zipper" effect (curtains)
    if (_transitionProgress > 0.0f)
    {
        int h = (int)(_transitionProgress * 32); // Half the screen
        u8g2.setDrawColor(0);                    // Black to erase
        u8g2.drawBox(0, 0, 128, h);              // Top curtain
        u8g2.drawBox(0, 64 - h, 128, h);         // Bottom curtain
        u8g2.setDrawColor(1);                    // Back to normal white
    }

    // 4. Particles (always visible)
    _animations.update(deltaTime);
    _animations.draw();

    // ... rest of the debug and rendering code ...

    // 4. Debug overlay
    if (_debugText.length() > 0)
    {
        u8g2.setFont(u8g2_font_5x7_tf);
        u8g2.setDrawColor(1);
        u8g2.drawStr(2, 8, _debugText.c_str());
    }

    u8g2.sendBuffer();
}

void DisplayOrchestrator::showInstruction(
    const uint8_t* icon,
    unsigned long durationMs
)
{
    _pendingIcon = icon;
    _instructionEndTime = (durationMs > 0) ? (millis() + durationMs) : 0;
    _transitionState = TransitionState::CLOSING;
}

void DisplayOrchestrator::clearInstruction()
{
    _pendingIcon = nullptr;
    _instructionEndTime = 0;

    if (_currentInstructionIcon != nullptr &&
        _transitionState == TransitionState::IDLE)
    {
        _transitionState = TransitionState::CLOSING;
    }
}

/**
 * @section Facial Expression Commands
 */

void DisplayOrchestrator::setEyeMood(eEmotions mood)
{
    // If we change the mood while an icon is active, close the zipper
    // first!
    if (_currentInstructionIcon != nullptr || _pendingIcon != nullptr)
    {
        _pendingIcon = nullptr;
        _transitionState = TransitionState::CLOSING;
    }
    _eyes.setMood(mood);
}

void DisplayOrchestrator::lookAt(float x, float y) { _eyes.lookAt(x, y); }

void DisplayOrchestrator::playConfused() { _eyes.playConfused(); }

void DisplayOrchestrator::playHappy() { _eyes.playHappy(); }

/**
 * @section Particle System Management
 * Controls the atmospheric effects that set the mood for the wash ritual.
 */

void DisplayOrchestrator::setParticleEffect(EffectType type)
{
    _animations.setEffect(type);
}

void DisplayOrchestrator::setDebugText(const String& text)
{
    _debugText = text;
}

/**
 * @section Power Management
 * Allows putting the display into power-save mode (off) or waking it up.
 */

void DisplayOrchestrator::prepareForSleep()
{
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    u8g2.setPowerSave(1);
    Serial.println(
        F("[DISPLAY] Memoria limpa e driver em modo economia (Display OFF).")
    );
}
