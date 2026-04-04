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

/** @section Ciclo de Vida */

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
    // Alocação estática dos estados para evitar fragmentação de memória durante
    // o ritual pedagógico.
    initializeStates();
}

GameController::~GameController()
{
    // Limpeza de memória: deleta todas as instâncias de estado alocadas via
    // 'new' no pool de estados.
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
    // Inicia a FSM no estado de Boot para sincronização de hardware.
    changeState(RobotState::BOOT);
}

/** @section Processamento de Loop (Update) */

void GameController::update()
{
    // Se o robô está em processo de desligamento, suspende todo o processamento
    // lógico.
    if (_isShuttingDown)
        return;

    // Limpa mensagens temporárias de debug após o tempo expirar.
    if (_debugTextClearTime > 0 && millis() > _debugTextClearTime)
    {
        _display.setDebugText("");
        _debugTextClearTime = 0;
    }

    // Processa a lógica do botão de debug de forma independente da FSM.
    processDebugButton();

    // --- MOTOR DE COMPORTAMENTO (VIDA ORGÂNICA) ---
    // O motor é pausado automaticamente se houver um ícone/instrução na tela
    // para não distrair a criança da tarefa pedagógica.
    _behaviors.setPaused(_display.isInstructionActive());
    _behaviors.update(_display, _motion, _audio);

    // Ciclo de vida: Atualização lógica do estado atual (Update Loop)
    if (_currentState)
    {
        _currentState->update(this);
    }
}

/** @section Interação Humano-Robô (Entradas) */

void GameController::processDebugButton()
{
    bool isPressed = digitalRead(Pins::BUTTON_DEBUG) == HIGH;

    if (isPressed)
    {
        if (!_buttonWasPressed)
        {
            // Detecção de borda de subida (Botão pressionado agora)
            _buttonPressTime = millis();
            _buttonWasPressed = true;
            _debugToggleHandled = false;
        }
        else if (!_debugToggleHandled)
        {
            // O botão continua sendo segurado pelo usuário.
            unsigned long pressDuration = millis() - _buttonPressTime;

            if (pressDuration >= GameConfig::DEBUG_LONG_PRESS_MS)
            {
                // Toggle do Modo Debug acontece imediatamente ao atingir o
                // tempo limite configurado.
                _isDebugMode = !_isDebugMode;
                _debugToggleHandled = true;

                if (_isDebugMode)
                {
                    Serial.println(F("[DEBUG] Modo Debug ATIVADO."));
                    _debugTextClearTime = 0;

                    // Força a tela a mostrar que entrou em debug antes da
                    // transição.
                    _display.setDebugText(
                        String(F("DEBUG: ")) + getStateName(RobotState::IDLE)
                    );

                    // Retorna ao Idle por segurança ao entrar em modo técnico.
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
            // Feedback visual de "Aguarde" enquanto o usuário segura o botão.
            else if (!_isDebugMode && pressDuration > GameConfig::DEBUG_HOLD_FEEDBACK_MS)
            {
                _display.setDebugText(F("HOLD..."));
                _debugTextClearTime =
                    millis() + GameConfig::DEBUG_MSG_DURATION_MS;
            }
        }
    }
    else if (_buttonWasPressed)
    {
        // Detecção de borda de descida (Botão solto agora)
        unsigned long pressDuration = millis() - _buttonPressTime;
        _buttonWasPressed = false;

        // Limpa o texto de "HOLD..." se o usuário desistiu de segurar.
        if (!_debugToggleHandled && !_isDebugMode &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS &&
            pressDuration < GameConfig::DEBUG_LONG_PRESS_MS)
        {
            _display.setDebugText("");
            _debugTextClearTime = 0;
        }

        // Processamento de clique curto (Short Press) para pular estados em
        // modo técnico.
        if (!_debugToggleHandled &&
            pressDuration > GameConfig::DEBUG_DEBOUNCE_MS)
        {
            if (_isDebugMode)
            {
                int currentStateInt = static_cast<int>(getCurrentStateEnum());
                // Avança ciclicamente no pool de estados.
                int nextStateInt = (currentStateInt + 1) %
                                   static_cast<int>(RobotState::STATE_COUNT);
                RobotState nextState = static_cast<RobotState>(nextStateInt);

                Serial.print(F("[DEBUG] Forçando transição manual para: "));
                Serial.println(getStateName(nextState));

                changeState(nextState);
            }
            else
            {
                Serial.println(F("[DEBUG] Botão pressionado (Segure para "
                                 "entrar no modo técnico)."));
            }
        }
    }
}

void GameController::processRFIDTag(const String& uid)
{
    if (_isShuttingDown)
        return;

    // Proteção: Ignora interações físicas se o robô estiver sob teste técnico.
    if (_isDebugMode)
    {
        Serial.println(F("[DEBUG] Tag ignorada."));
        return;
    }

    // Proteção Pedagógica: Ignora tags enquanto uma etapa obrigatória está em
    // curso.
    if (isRitualState(getCurrentStateEnum()))
    {
        Serial.println(F("[FSM] Tag ignorada: Etapa em andamento."));
        return;
    }

    Serial.print(F("[RFID] Tag Detectada: "));
    Serial.println(uid);

    // Delega o tratamento da tag para o estado atual da FSM.
    if (_currentState)
    {
        _currentState->handleRFID(this, uid);
    }
}

/** @section Gerenciamento da Máquina de Estados (FSM) */

void GameController::changeState(RobotState stateEnum)
{
    // Lógica Pedagógica: Gerenciamento de Repetições
    // Previne que a criança fique presa em um loop infinito de uma única etapa.
    if (isRitualState(stateEnum))
    {
        if (stateEnum != _lastRitualState)
        {
            // Iniciou uma nova etapa: reseta contadores de progresso local.
            resetRitualProgress();
            _lastRitualState = stateEnum;
        }
        else
        {
            // Tentativa de repetir a mesma etapa consecutivamente.
            _repeatCount++;

            // Se exceder o limite, redireciona para o estado de Erro/Atenção.
            if (_repeatCount > 1)
            {
                Serial.println(F("[FSM] Erro: Limite de repetição excedido."));
                stateEnum = RobotState::ERROR;
            }
        }
    }
    // Estados globais resetam o progresso para permitir novos ciclos de ritual.
    else if (stateEnum == RobotState::IDLE || stateEnum == RobotState::BOOT || stateEnum == RobotState::SUCCESS)
    {
        resetRitualProgress();
    }

    // Resolve o ponteiro do estado via pool estático e aplica a transição.
    int index = static_cast<int>(stateEnum);
    changeState(_states[index]);
}

void GameController::changeState(State* newState)
{
    // Sincroniza o overlay de debug com o nome do estado atual.
    if (_isDebugMode && newState != nullptr)
    {
        _display.setDebugText(
            String(F("DEBUG: ")) + getStateName(newState->getStateEnum())
        );
    }

    // Otimização: Evita transições para o mesmo estado (exceto rituais
    // permitidos).
    if (_currentState == newState && !isRitualState(newState->getStateEnum()))
        return;

    // --- CICLO DE VIDA: SAÍDA (EXIT) ---
    if (_currentState != nullptr)
    {
        _currentState->exit(this);
        _previousState = _currentState;
    }

    // --- ATUALIZAÇÃO DE CONTEXTO ---
    _currentState = newState;
    _stateStartTime = millis();

    // Log padronizado de transição de estado.
    Serial.print(F("[FSM] Transição -> "));
    Serial.print(getStateName(_currentState->getStateEnum()));
    if (isRitualState(_currentState->getStateEnum()))
    {
        Serial.print(F(" (Ciclo: "));
        Serial.print(_repeatCount + 1);
        Serial.print(F(")"));
    }
    Serial.println();

    // --- RESET DE HARDWARE E VISUAL PARA NOVO ESTADO ---
    // Limpa instruções, partículas e sons do estado anterior para evitar
    // poluição sensorial.
    _display.clearInstruction();
    _display.setParticleEffect(EffectType::NONE);
    _audio.stop();
    _motion.stopAllAnimations();
    _motion.centerAll();

    // --- CICLO DE VIDA: ENTRADA (ENTER) ---
    if (_currentState != nullptr)
    {
        _currentState->enter(this);
    }
}

void GameController::handleRepeat()
{
    // Força o retorno ao último estado pedagógico válido.
    changeState(_lastRitualState);
}

/** @section Utilitários de Diagnóstico e Controle */

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
    // Define o escopo das etapas de higienização ativa.
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

/** @section Gerenciamento de Energia e Desligamento */

extern bool isSystemSleeping;

void GameController::shutdownSystem()
{
    if (_isShuttingDown)
        return;
    _isShuttingDown = true;

    Serial.println(F("[SYSTEM] Iniciando desligamento seguro..."));

    // 1. Sinaliza parada imediata para tarefas assíncronas (Core 0/1).
    isSystemSleeping = true;
    delay(100);

    // 2. Congela o processamento da FSM e motor orgânico imediatamente.
    _currentState = nullptr;
    _behaviors.stop();

    // 3. Comando de centralização (agendamento).
    _motion.stopAllAnimations();
    _motion.centerAll();

    // 4. Silêncio absoluto: Desliga o amplificador do hardware.
    _audio.hibernate();

    // 5. Apaga e desliga o controlador OLED.
    _display.prepareForSleep();

    // 6. LOOP DE ESPERA ATIVA PARA MOTORES
    // Como o loop() principal está travado por esta função, precisamos
    // atualizar a física dos motores manualmente aqui para que eles se movam.
    Serial.println(F("[SYSTEM] Centralizando motores..."));
    unsigned long startCenter = millis();
    unsigned long lastUpdate = startCenter;

    while (millis() - startCenter < 1500)
    {
        unsigned long now = millis();
        float dt = (now - lastUpdate) / 1000.0f;
        lastUpdate = now;

        // Processa a cinemática dos servos manualmente
        _motion.update(dt);
        delay(10); // Pequena pausa para manter a cadência
    }

    // 7. Envia o sinal final para o módulo de bateria desligar tudo.
    _power.requestSystemShutdown();
}
