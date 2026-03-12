#include "GameController.h"
#include "DisplayManager.h"
#include "MotionManager.h"
#include "RFIDManager.h"

GameController::GameController(
    DisplayManager& display,
    MotionManager& motion,
    RFIDManager& rfid
)
    : _display(display), _motion(motion), _rfid(rfid),
      _currentState(RobotState::BOOT), _stateStartTime(0)
{
}

void GameController::init()
{
    // O robô acabou de ligar. Começa no estado BOOT.
    changeState(RobotState::BOOT);
}

void GameController::update()
{
    // 1. OUVIR O MUNDO: Verifica se a criança passou algum cartão novo
    if (_rfid.isCardPresent())
    {
        String uid = _rfid.readCardUID();
        if (uid.length() > 0)
        {
            Serial.print("[GameController] Cartão detetado: ");
            Serial.println(uid);
            processRFIDTag(uid);
        }
    }

    // 2. PENSAR: O que o robô deve fazer dependendo do estado atual?
    // TODO: A sua equipa deve expandir este switch com os tempos de cada estado!
    switch (_currentState)
    {
        case RobotState::BOOT:
            // Passados 3 segundos no BOOT, vai para o IDLE esperar uma criança
            if (millis() - _stateStartTime > 3000)
            {
                changeState(RobotState::IDLE);
            }
            break;

        case RobotState::IDLE:
            handleIdleState();
            break;

        case RobotState::WET:
        case RobotState::SOAP:
        case RobotState::SCRUB:
        case RobotState::RINSE:
        case RobotState::DRY:
            handleWashingState();
            break;

        case RobotState::SUCCESS:
            // Celebra por 5 segundos e volta ao IDLE
            if (millis() - _stateStartTime > 5000)
            {
                changeState(RobotState::IDLE);
            }
            break;

        case RobotState::ERROR:
            // Fica zangado por 4 segundos e volta ao IDLE
            if (millis() - _stateStartTime > 4000)
            {
                changeState(RobotState::IDLE);
            }
            break;
    }
}

void GameController::changeState(RobotState newState)
{
    if (_currentState == newState)
        return; // Não faz nada se tentar mudar para o mesmo estado

    _currentState = newState;
    _stateStartTime = millis();

    Serial.print("[GameController] Mudou de estado para: ");
    Serial.println(static_cast<int>(newState));

    // AQUI É ONDE O HARDWARE REAGE IMEDIATAMENTE À MUDANÇA!
    switch (newState)
    {
        case RobotState::BOOT:
            _display.setEyeMood(EyeMood::DEFAULT);
            _display.setParticleEffect(EffectType::NONE);
            _motion.centerAll();
            break;

        case RobotState::IDLE:
            _display.setEyeMood(EyeMood::DEFAULT);
            _display.setEyeIdleMode(true);
            _display.setParticleEffect(EffectType::NONE);
            _motion.lookLeft();
            _motion.moveArmL(120);
            break;

        case RobotState::WET:
            _display.setEyeMood(EyeMood::TIRED);
            _display.setParticleEffect(EffectType::RAIN_LIGHT);
            _motion.lookRight();
            _motion.moveArmR(60);
            break;

        case RobotState::SOAP:
            _display.lookAt(EyePosition::N);
            _display.setParticleEffect(EffectType::BUBBLES);
            _motion.centerAll();
            break;

        case RobotState::SCRUB:
            _display.setEyeMood(EyeMood::HAPPY);
            _display.setParticleEffect(EffectType::BUBBLES);
            _motion.moveArmL(160, 20.0f);
            _motion.moveArmR(20, 20.0f);
            break;

        case RobotState::RINSE:
            _display.lookAt(EyePosition::S);
            _display.setParticleEffect(EffectType::RAIN_HEAVY);
            _motion.centerAll();
            break;

        case RobotState::DRY:
            _display.setEyeMood(EyeMood::TIRED);
            _display.setParticleEffect(EffectType::WIND);
            _motion.lookLeft();
            break;

        case RobotState::SUCCESS:
            _display.playHappy();
            _display.setParticleEffect(EffectType::CONFETTI);
            _motion.waveHands();
            break;

        case RobotState::ERROR:
            _display.setEyeMood(EyeMood::ANGRY);
            _display.playConfused();
            _display.setParticleEffect(EffectType::NONE);
            _motion.moveHead(60, 30.0f);
            _motion.moveArmL(180, 20.0f);
            break;
    }
}

void GameController::processRFIDTag(const String& uid)
{
    // TODO: Adicione aqui os UIDs dos vossos cartões reais!
    // Exemplo:
    if (uid == "01:02:03:04") 
    {
        // Se for o cartão "01:02:03:04", começa a lavagem (vai para WET)
        changeState(RobotState::WET);
    }
    else if (uid == "11:22:33:44")
    {
    }
    else if (uid == "55:66:77:88")
    {
    }
    else if (uid == "AA:BB:CC:DD")
    {
    }
}

void GameController::handleIdleState()
{
    // TODO: No estado IDLE, o robô está à espera de um cartão.
    // Pode querer fazer uma animação de "chamar a atenção" se passar muito tempo.
    // unsigned long timeInIdle = millis() - _stateStartTime;
    // if(timeInIdle > 10000) { _motion.waveHands(); }
}

void GameController::handleWashingState()
{
    // TODO: Fazer a lógica de transição automática aqui.
    // Exemplo: WET dura 5s, depois muda sozinho para SOAP.
    unsigned long elapsedTime = millis() - _stateStartTime;

    if (_currentState == RobotState::WET && elapsedTime > 5000)
    {
        changeState(RobotState::SOAP);
    }
    else if (_currentState == RobotState::SOAP && elapsedTime > 5000)
    {
        changeState(RobotState::SCRUB);
    }
    else if (_currentState == RobotState::SCRUB && elapsedTime > 10000)
    {
        changeState(RobotState::RINSE);
    }
    else if (_currentState == RobotState::RINSE && elapsedTime > 5000)
    {
        changeState(RobotState::DRY);
    }
    else if (_currentState == RobotState::DRY && elapsedTime > 5000)
    {
        changeState(RobotState::SUCCESS);
    }
}