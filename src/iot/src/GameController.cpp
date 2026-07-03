#include "GameController.h"
#include "states/BootState.h"
#include "states/DryState.h"
#include "states/ErrorState.h"
#include "states/IdleState.h"
#include "states/RinseState.h"
#include "states/ScrubState.h"
#include "states/SoapState.h"
#include "states/SuccessState.h"
#include "states/WaitingState.h"
#include "states/WetState.h"

/** @section Lifecycle */

GameController::GameController(
    DisplayOrchestrator& display,
    MotionController& motion,
    RFIDReader& rfid,
    AudioController& audio,
    PowerController& power
)
    : _display(display), _motion(motion), _rfid(rfid), _audio(audio),
      _power(power), _currentState(nullptr), _previousState(nullptr),
      _lastRitualState(RobotState::BOOT), _stateStartTime(0), _repeatCount(0)
{
    // Static allocation of the states to avoid memory fragmentation during
    // the pedagogical ritual.
    initializeStates();
}

GameController::~GameController()
{
    // Memory cleanup: deletes every state instance allocated via 'new' in
    // the state pool.
    for (int i = 0; i < static_cast<int>(RobotState::STATE_COUNT); i++)
    {
        if (_states[i] != nullptr)
        {
            delete _states[i];
            _states[i] = nullptr;
        }
    }
}

void GameController::initializeStates()
{
    _states[static_cast<int>(RobotState::BOOT)] = new BootState();
    _states[static_cast<int>(RobotState::IDLE)] = new IdleState();
    _states[static_cast<int>(RobotState::WET)] = new WetState();
    _states[static_cast<int>(RobotState::SOAP)] = new SoapState();
    _states[static_cast<int>(RobotState::SCRUB)] = new ScrubState();
    _states[static_cast<int>(RobotState::RINSE)] = new RinseState();
    _states[static_cast<int>(RobotState::DRY)] = new DryState();
    _states[static_cast<int>(RobotState::SUCCESS)] = new SuccessState();
    _states[static_cast<int>(RobotState::ERROR)] = new ErrorState();
    _states[static_cast<int>(RobotState::WAITING)] = new WaitingState();
}

void GameController::init()
{
    // Starts the FSM in the Boot state to synchronize hardware.
    changeState(RobotState::BOOT);
}

/** @section Loop Processing (Update) */

void GameController::update()
{
    // If the robot is shutting down, suspend all logic processing.
    if (_isShuttingDown)
        return;

    // Clears temporary debug messages once their time expires.
    if (_debugTextClearTime > 0 && millis() > _debugTextClearTime)
    {
        _display.setDebugText("");
        _debugTextClearTime = 0;
    }

    // Processes the debug button logic independently from the FSM.
    processDebugButton();

    // --- BEHAVIOR ENGINE (ORGANIC LIFE) ---
    // The engine is automatically paused whenever an icon/instruction is on
    // screen, so it never distracts the child from the pedagogical task.
    _behaviors.setPaused(_display.isInstructionActive());
    _behaviors.update(_display, _motion, _audio);

    // Lifecycle: logic update of the current state (Update Loop)
    if (_currentState)
    {
        _currentState->update(this);
    }
}

/** @section Human-Robot Interaction (Inputs) */

void GameController::processDebugButton()
{
    bool isPressed = digitalRead(Pins::BUTTON_DEBUG) == HIGH;

    if (isPressed)
    {
        if (!_buttonWasPressed)
        {
            // Rising edge detection (button pressed now)
            _buttonPressTime = millis();
            _buttonWasPressed = true;
            _debugToggleHandled = false;
        }
        else if (!_debugToggleHandled)
        {
            // The button is still being held down.
            unsigned long pressDuration = millis() - _buttonPressTime;

            if (pressDuration >= GameConfig::DEBUG_LONG_PRESS_MS)
            {
                // Debug mode toggles immediately once the configured
                // threshold is reached.
                _isDebugMode = !_isDebugMode;
                _debugToggleHandled = true;

                if (_isDebugMode)
                {
                    Serial.println(F("[DEBUG] Modo Debug ATIVADO."));
                    _debugTextClearTime = 0;

                    // Forces the screen to show that debug mode was entered
                    // before the transition happens.
                    _display.setDebugText(
                        String(F("DEBUG: ")) + getStateName(RobotState::IDLE)
                    );

                    // Returns to Idle for safety when entering technical
                    // mode.
                    changeState(RobotState::IDLE);
                }
                else
                {
                    Serial.println(F("[DEBUG] Modo Debug DESATIVADO."));
                    _display.setDebugText(F("DEBUG OFF"));
                    _debugTextClearTime =
                        millis() + GameConfig::DEBUG_MSG_DURATION_MS;
                    changeState(RobotState::IDLE);
                }
            }
            // Visual "Hold" feedback while the user keeps the button
            // pressed.
            else if (
                !_isDebugMode &&
                pressDuration > GameConfig::DEBUG_HOLD_FEEDBACK_MS
            )
            {
                _display.setDebugText(F("HOLD..."));
                _debugTextClearTime =
                    millis() + GameConfig::DEBUG_MSG_DURATION_MS;
            }
        }
    }
    else if (_buttonWasPressed)
    {
        // Falling edge detection (button released now)
        unsigned long pressDuration = millis() - _buttonPressTime;
        _buttonWasPressed = false;

        // Clears the "HOLD..." text if the user gave up holding the button.
        if (!_debugToggleHandled && !_isDebugMode &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS &&
            pressDuration < GameConfig::DEBUG_LONG_PRESS_MS)
        {
            _display.setDebugText("");
            _debugTextClearTime = 0;
        }

        // Short-press processing to skip states while in technical mode.
        if (!_debugToggleHandled &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS)
        {
            if (_isDebugMode)
            {
                int currentStateInt = static_cast<int>(getCurrentStateEnum());
                // Advances cyclically through the state pool.
                int nextStateInt = (currentStateInt + 1) %
                                   static_cast<int>(RobotState::STATE_COUNT);
                RobotState nextState = static_cast<RobotState>(nextStateInt);

                Serial.print(F("[DEBUG] Forçando transição manual para: "));
                Serial.println(getStateName(nextState));

                changeState(nextState);
            }
            else
            {
                Serial.println(
                    F("[DEBUG] Botão pressionado (Segure para "
                      "entrar no modo técnico).")
                );
            }
        }
    }
}

void GameController::processRFIDTag(const String& uid)
{
    if (_isShuttingDown)
        return;

    // Guard: ignore physical interactions while the robot is under
    // technical testing.
    if (_isDebugMode)
    {
        Serial.println(F("[DEBUG] Tag ignorada."));
        return;
    }

    // Pedagogical guard: ignore tags while a mandatory stage is in progress.
    if (isRitualState(getCurrentStateEnum()))
    {
        Serial.println(F("[FSM] Tag ignorada: Etapa em andamento."));
        return;
    }

    Serial.print(F("[RFID] Tag Detectada: "));
    Serial.println(uid);

    // Delegates the tag handling to the current FSM state.
    if (_currentState)
    {
        _currentState->handleRFID(this, uid);
    }
}

/** @section State Machine (FSM) Management */

void GameController::changeState(RobotState stateEnum)
{
    // Pedagogical logic: repeat management.
    // Prevents the child from getting stuck in an infinite loop on a single
    // stage.
    if (isRitualState(stateEnum))
    {
        if (stateEnum != _lastRitualState)
        {
            // A new stage started: resets the local progress counters.
            resetRitualProgress();
            _lastRitualState = stateEnum;
        }
        else
        {
            // Attempt to repeat the same stage consecutively.
            _repeatCount++;

            // If the limit is exceeded, redirect to the Error/Attention
            // state.
            if (_repeatCount > 1)
            {
                Serial.println(F("[FSM] Erro: Limite de repetição excedido."));
                stateEnum = RobotState::ERROR;
            }
        }
    }
    // Global states reset the progress to allow new ritual cycles.
    else if (
        stateEnum == RobotState::IDLE || stateEnum == RobotState::BOOT ||
        stateEnum == RobotState::SUCCESS
    )
    {
        resetRitualProgress();
    }

    // Resolves the state pointer via the static pool and applies the
    // transition.
    int index = static_cast<int>(stateEnum);
    changeState(_states[index]);
}

void GameController::changeState(State* newState)
{
    // Synchronizes the debug overlay with the current state's name.
    if (_isDebugMode && newState != nullptr)
    {
        _display.setDebugText(
            String(F("DEBUG: ")) + getStateName(newState->getStateEnum())
        );
    }

    // Optimization: avoid transitioning into the same state (except for
    // allowed ritual repeats).
    if (_currentState == newState && !isRitualState(newState->getStateEnum()))
        return;

    // --- LIFECYCLE: EXIT ---
    if (_currentState != nullptr)
    {
        _currentState->exit(this);
        _previousState = _currentState;
    }

    // --- CONTEXT UPDATE ---
    _currentState = newState;
    _stateStartTime = millis();

    // Standardized transition log.
    Serial.print(F("[FSM] Transição -> "));
    Serial.print(getStateName(_currentState->getStateEnum()));
    if (isRitualState(_currentState->getStateEnum()))
    {
        Serial.print(F(" (Ciclo: "));
        Serial.print(_repeatCount + 1);
        Serial.print(F(")"));
    }
    Serial.println();

    // --- HARDWARE AND VISUAL RESET FOR THE NEW STATE ---
    // Clears instructions, particles, and sounds from the previous state to
    // avoid sensory clutter.
    _display.clearInstruction();
    _display.setParticleEffect(EffectType::NONE);
    _audio.stop();
    _motion.stopAllAnimations();
    _motion.centerAll();

    // --- LIFECYCLE: ENTER ---
    if (_currentState != nullptr)
    {
        _currentState->enter(this);
    }
}

void GameController::handleRepeat()
{
    // Forces a return to the last valid pedagogical state.
    changeState(_lastRitualState);
}

/** @section Diagnostics and Control Utilities */

void GameController::resetRitualProgress()
{
    _repeatCount = 0;
    _lastRitualState = RobotState::BOOT;
}

const char* GameController::getStateName(RobotState state)
{
    switch (state)
    {
        case RobotState::BOOT:
            return "BOOT";
        case RobotState::IDLE:
            return "IDLE";
        case RobotState::WET:
            return "WET";
        case RobotState::SOAP:
            return "SOAP";
        case RobotState::SCRUB:
            return "SCRUB";
        case RobotState::RINSE:
            return "RINSE";
        case RobotState::DRY:
            return "DRY";
        case RobotState::SUCCESS:
            return "SUCCESS";
        case RobotState::ERROR:
            return "ERROR";
        case RobotState::WAITING:
            return "WAITING";
        default:
            return "UNKNOWN";
    }
}

bool GameController::isRitualState(RobotState state)
{
    // Defines the scope of the active hygiene stages.
    return (
        state == RobotState::WET || state == RobotState::SOAP ||
        state == RobotState::SCRUB || state == RobotState::RINSE ||
        state == RobotState::DRY
    );
}

RobotState GameController::getCurrentStateEnum() const
{
    if (_currentState)
        return _currentState->getStateEnum();
    return RobotState::BOOT;
}

/** @section Power and Shutdown Management */

extern bool isSystemSleeping;

void GameController::shutdownSystem()
{
    if (_isShuttingDown)
        return;
    _isShuttingDown = true;

    Serial.println(F("[SYSTEM] Iniciando desligamento seguro..."));

    // 1. Signals the async tasks (Core 0/1) to stop immediately.
    isSystemSleeping = true;
    delay(100);

    // 2. Freezes FSM and behavior engine processing right away.
    _currentState = nullptr;
    _behaviors.stop();

    // 3. Centering command (scheduled).
    _motion.stopAllAnimations();
    _motion.centerAll();

    // 4. Absolute silence: turns off the hardware amplifier.
    _audio.hibernate();

    // 5. Clears and turns off the OLED controller.
    _display.prepareForSleep();

    // 6. ACTIVE WAIT LOOP FOR THE MOTORS
    // Since the main loop() is blocked by this function, we need to update
    // the motor physics manually here so they can actually move.
    Serial.println(F("[SYSTEM] Centralizando motores..."));
    unsigned long startCenter = millis();
    unsigned long lastUpdate = startCenter;

    while (millis() - startCenter < 1500)
    {
        unsigned long now = millis();
        float dt = (now - lastUpdate) / 1000.0f;
        lastUpdate = now;

        // Manually processes the servo kinematics
        _motion.update(dt);
        delay(10); // Small pause to keep the cadence
    }

    // 7. Sends the final signal to the battery module to power everything
    // down.
    _power.requestSystemShutdown();
}
