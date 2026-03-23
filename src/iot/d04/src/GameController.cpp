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

/** @section Ciclo de Vida */

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

GameController::~GameController()
{
    // Limpeza de memória: deleta todas as instâncias de estado alocadas via
    // 'new'
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
    _states[static_cast<int>(RobotState::SLEEP)] = new SleepState();
}

void GameController::init()
{
    // Inicia a FSM no estado de Boot
    changeState(RobotState::BOOT);
}

/** @section Processamento */

void GameController::update()
{
    // Limpa mensagens temporárias de debug após o tempo expirar
    if (_debugTextClearTime > 0 && millis() > _debugTextClearTime)
    {
        _display.setDebugText("");
        _debugTextClearTime = 0;
    }

    // Processa a lógica do botão de debug de forma independente
    processDebugButton();

    // --- MOTOR DE COMPORTAMENTO (VIDA ORGÂNICA) ---
    // O motor é pausado automaticamente se houver um ícone/instrução na tela
    _behaviors.setPaused(_display.isInstructionActive());
    _behaviors.update(_display, _motion);

    // Ciclo de vida: Atualização lógica do estado atual
    if (_currentState)
    {
        _currentState->update(this);
    }
}

void GameController::processDebugButton()
{
    bool isPressed = digitalRead(Pins::BUTTON_DEBUG) == HIGH;

    if (isPressed)
    {
        if (!_buttonWasPressed)
        {
            // Botão acabou de ser pressionado
            _buttonPressTime = millis();
            _buttonWasPressed = true;
            _debugToggleHandled = false;
        }
        else if (!_debugToggleHandled)
        {
            // O botão continua pressionado
            unsigned long pressDuration = millis() - _buttonPressTime;

            if (pressDuration >= GameConfig::DEBUG_LONG_PRESS_MS)
            {
                // Toggle do Modo Debug acontece imediatamente ao atingir o
                // tempo
                _isDebugMode = !_isDebugMode;
                _debugToggleHandled =
                    true; // Impede que fique ativando em loop enquanto segura

                if (_isDebugMode)
                {
                    Serial.println(F("[DEBUG] Entrando no modo Debug."));
                    _debugTextClearTime =
                        0; // Se está em debug, o texto fica fixo (será setado
                           // na transição de estado)

                    // Força a tela a mostrar que entrou em debug antes da
                    // transição (que pode ser muito rápida)
                    _display.setDebugText(
                        String("DEBUG: ") + getStateName(RobotState::IDLE)
                    );

                    // Vai para Idle (isso chamará changeState, mas precisamos
                    // garantir que o texto não fique preso em "HOLD...")
                    changeState(RobotState::IDLE);
                }
                else
                {
                    Serial.println(F("[DEBUG] Saindo do modo Debug."));
                    _display.setDebugText("DEBUG OFF");
                    _debugTextClearTime =
                        millis() +
                        GameConfig::DEBUG_MSG_DURATION_MS; // Apaga o aviso após
                                                           // o tempo
                                                           // configurado
                    changeState(RobotState::IDLE);
                }
            }
            // Feedback visual intermediário se estiver segurando para ativar
            // (opcional)
            else if (
                !_isDebugMode &&
                pressDuration > GameConfig::DEBUG_HOLD_FEEDBACK_MS
            )
            {
                _display.setDebugText("HOLD...");
                _debugTextClearTime =
                    millis() +
                    GameConfig::DEBUG_MSG_DURATION_MS; // Proteção para apagar o
                                                       // HOLD caso o loop
                                                       // atrase
            }
        }
    }
    else if (_buttonWasPressed)
    {
        // Botão foi solto
        unsigned long pressDuration = millis() - _buttonPressTime;
        _buttonWasPressed = false;

        // Limpa o texto de "HOLD..." imediatamente se o usuário desistiu de
        // segurar antes do tempo
        if (!_debugToggleHandled && !_isDebugMode &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS &&
            pressDuration < GameConfig::DEBUG_LONG_PRESS_MS)
        {
            _display.setDebugText("");
            _debugTextClearTime = 0;
        }

        // Se soltou e o tempo foi curto, processa como um clique (Short Press)
        // A flag _debugToggleHandled garante que se foi um long press, não vai
        // engatilhar isso.
        if (!_debugToggleHandled &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS)
        {
            if (_isDebugMode)
            {
                int currentStateInt = static_cast<int>(getCurrentStateEnum());
                // Usa o STATE_COUNT dinâmico para não quebrar se adicionar
                // novos estados
                int nextStateInt = (currentStateInt + 1) %
                                   static_cast<int>(RobotState::STATE_COUNT);
                RobotState nextState = static_cast<RobotState>(nextStateInt);

                Serial.print(F("[DEBUG] Forçando transição para: "));
                Serial.println(getStateName(nextState));

                changeState(nextState);
            }
            else
            {
                Serial.println(
                    F("[DEBUG] Botão pressionado, mas modo Debug "
                      "está inativo. (Segure para ativar)")
                );
            }
        }
    }
}

void GameController::processRFIDTag(const String& uid)
{
    // Proteção: Ignora tags se o robô estiver sob teste manual
    if (_isDebugMode)
    {
        Serial.println(F("[DEBUG] Tag ignorada. Modo debug ativo."));
        return;
    }

    // Proteção Pedagógica: Ignora tags durante a execução de etapas do ritual.
    // Isso garante que a criança complete a ação (ex: esfregar) pelo tempo
    // determinado antes de poder avançar ou repetir.
    if (isRitualState(getCurrentStateEnum()))
    {
        Serial.println(
            F("[GameController] Tag ignorada: Etapa do ritual em andamento.")
        );
        return;
    }

    Serial.print(F("[GameController] Tag Detectada: "));
    Serial.println(uid);

    // Delega o tratamento da tag para o estado atual (Strategy Pattern)
    if (_currentState)
    {
        _currentState->handleRFID(this, uid);
    }
}

/** @section Gerenciamento de Estados */

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
    else if (
        stateEnum == RobotState::IDLE || stateEnum == RobotState::BOOT ||
        stateEnum == RobotState::SUCCESS
    )
    {
        resetRitualProgress();
    }

    // Busca a instância do estado no pool e realiza a transição
    int index = static_cast<int>(stateEnum);
    changeState(_states[index]);
}

void GameController::changeState(State* newState)
{
    // Sincroniza o texto de debug SEMPRE que o estado tenta mudar,
    // mesmo que a transição seja redundante (como tentar ir para IDLE estando
    // em IDLE)
    if (_isDebugMode && newState != nullptr)
    {
        _display.setDebugText(
            String("DEBUG: ") + getStateName(newState->getStateEnum())
        );
    }

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
    _motion.centerAll();

    // Ciclo de vida: Entrada no novo estado
    if (_currentState != nullptr)
    {
        _currentState->enter(this);
    }
}

void GameController::handleRepeat()
{
    // Aciona a transição para o último estado ritualístico registrado.
    // A lógica de contagem e erro será processada dentro do
    // changeState(RobotState).
    changeState(_lastRitualState);
}

/** @section Ações Auxiliares */

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

RobotState GameController::getCurrentStateEnum() const
{
    if (_currentState)
        return _currentState->getStateEnum();
    return RobotState::BOOT;
}
