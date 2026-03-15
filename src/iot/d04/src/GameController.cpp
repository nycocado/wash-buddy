#include "GameController.h"
#include "states/BootState.h"
#include "states/DryState.h"
#include "states/ErrorState.h"
#include "states/IdleState.h"
#include "states/RinseState.h"
#include "states/ScrubState.h"
#include "states/SleepState.h"
#include "states/SoapState.h"
#include "states/SuccessState.h"
#include "states/WaitingState.h"
#include "states/WetState.h"

GameController::GameController(
    DisplayOrchestrator& display,
    MotionController& motion,
    RFIDReader& rfid
)
    : _display(display), _motion(motion), _rfid(rfid), _currentState(nullptr),
      _previousState(nullptr), _lastRitualState(RobotState::BOOT),
      _stateStartTime(0), _repeatCount(0)
{
    // Alocação estática dos estados para evitar fragmentação de memória durante
    // o ritual
    initializeStates();
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
    _states[static_cast<int>(RobotState::SLEEP)] = new SleepState();
}

void GameController::init()
{
    // Inicia a FSM no estado de Boot
    changeState(RobotState::BOOT);
}

void GameController::update()
{
    // Hardware: Polling do Leitor RFID
    if (_rfid.isCardPresent())
    {
        String uid = _rfid.readCardUID();
        if (uid.length() > 0)
        {
            Serial.print(F("[GameController] Tag Detectada: "));
            Serial.println(uid);

            // Delega o tratamento da tag para o estado atual (Strategy Pattern)
            if (_currentState)
            {
                _currentState->handleRFID(this, uid);
            }
        }
    }

    // Ciclo de vida: Atualização lógica do estado atual
    if (_currentState)
    {
        _currentState->update(this);
    }
}

void GameController::changeState(RobotState stateEnum)
{
    // Regra de Negócio: Gerenciamento de Repetições do Ritual
    // O objetivo é garantir que a criança siga a ordem, mas permitir uma
    // correção (repetição)
    if (isRitualState(stateEnum))
    {
        if (stateEnum != _lastRitualState)
        {
            // Se mudou para um NOVO estado do ritual, reseta o contador de
            // repetições
            resetRitualProgress();
            _lastRitualState = stateEnum;
        }
        else
        {
            // Se tentou entrar no MESMO estado ritualístico (ex: leu a tag de
            // sabão duas vezes)
            _repeatCount++;

            // Limite Pedagógico: Se repetir mais de uma vez a mesma etapa,
            // dispara erro. Isso evita que a criança fique "brincando" apenas
            // com uma etapa.
            if (_repeatCount > 1)
            {
                Serial.println(
                    F("[GameController] Erro: Limite de repeticao excedido.")
                );
                stateEnum = RobotState::ERROR;
            }
        }
    }
    // Estados neutros ou finais resetam o progresso para um novo ciclo
    else if (stateEnum == RobotState::IDLE || stateEnum == RobotState::BOOT || stateEnum == RobotState::SUCCESS)
    {
        resetRitualProgress();
    }

    // Busca a instância do estado no pool e realiza a transição
    int index = static_cast<int>(stateEnum);
    changeState(_states[index]);
}

void GameController::changeState(State* newState)
{
    // Evita transições redundantes (exceto para estados rituais que permitem
    // repetição controlada)
    if (_currentState == newState && !isRitualState(newState->getStateEnum()))
        return;

    // Ciclo de vida: Saída do estado anterior
    if (_currentState != nullptr)
    {
        _currentState->exit(this);
        _previousState = _currentState;
    }

    // Configuração do novo estado
    _currentState = newState;
    _stateStartTime = millis();

    // Log de depuração da transição
    Serial.print(F("[FSM Transition] -> "));
    Serial.print(getStateName(_currentState->getStateEnum()));
    if (isRitualState(_currentState->getStateEnum()))
    {
        Serial.print(F(" (Tentativa: "));
        Serial.print(_repeatCount + 1);
        Serial.print(F(")"));
    }
    Serial.println();

    // Reset de Hardware/Visual: Garante que o novo estado comece com "folha
    // limpa"
    _display.setEyeMood(eEmotions::Normal);
    _display.lookAt(0.0f, 0.0f);
    _display.setParticleEffect(EffectType::NONE);
    _motion.stopAllAnimations();

    // Ciclo de vida: Entrada no novo estado
    if (_currentState != nullptr)
    {
        _currentState->enter(this);
    }
}

void GameController::resetRitualProgress()
{
    _repeatCount = 0;
    _lastRitualState = RobotState::BOOT;
}

const char* GameController::getStateName(RobotState state) const
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
        case RobotState::SLEEP:
            return "SLEEP";
        default:
            return "UNKNOWN";
    }
}

bool GameController::isRitualState(RobotState state) const
{
    // Define quais estados compõem a jornada pedagógica de lavagem das mãos
    return (
        state == RobotState::WET || state == RobotState::SOAP ||
        state == RobotState::SCRUB || state == RobotState::RINSE ||
        state == RobotState::DRY
    );
}

void GameController::handleRepeat()
{
    // Aciona a transição para o último estado ritualístico registrado.
    // A lógica de contagem e erro será processada dentro do
    // changeState(RobotState).
    changeState(_lastRitualState);
}

RobotState GameController::getCurrentStateEnum() const
{
    if (_currentState)
        return _currentState->getStateEnum();
    return RobotState::BOOT;
}
