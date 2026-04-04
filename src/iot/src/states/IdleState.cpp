#include "states/IdleState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Define o Pool de Personalidade para o modo de repouso (Idle)
static const std::vector<BehaviorVignette> IDLE_POOL = {
    BehaviorVignette(
        eEmotions::Focused,
        0.0f,
        0.2f,
        ChoreoAction(ChoreographyLibrary::idlePanLook()),
        ChoreoAction(),
        ChoreoAction(),
        6300 // Tempo total do idlePanLook
    ),
    BehaviorVignette(
        eEmotions::Glee,
        0.0f,
        0.0f,
        ChoreoAction(),
        ChoreoAction(ChoreographyLibrary::waveHand(), 0.0f, true),
        ChoreoAction(),
        8500,
        AudioFiles::HIDE_AND_BEAK
    ),
    BehaviorVignette(
        eEmotions::Glee,
        0.0f,
        0.0f,
        ChoreoAction(),
        ChoreoAction(),
        ChoreoAction(ChoreographyLibrary::waveHand(), 0.0f, true),
        8500,
        AudioFiles::HIDE_AND_BEAK
    ),
    BehaviorVignette(
        eEmotions::Surprised,
        0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(70)),
        ChoreoAction(),
        ChoreoAction(),
        2000 // Tempo total do lookSide
    ),
    BehaviorVignette(
        eEmotions::Surprised,
        -0.7f,
        0.0f,
        ChoreoAction(ChoreographyLibrary::lookSide(110)),
        ChoreoAction(),
        ChoreoAction(),
        2000 // Tempo total do lookSide
    )};

/** @section Ciclo de Vida */

void IdleState::enter(GameController* controller)
{
    // Feedback sonoro de "Pronto", exceto se estiver voltando do WAITING
    // (timeout) para não ser repetitivo.
    State* prev = controller->getPreviousState();
    if (prev == nullptr || prev->getStateEnum() != RobotState::WAITING)
    {
        controller->getAudio().playFile(AudioFiles::IDLE_READY);
        // Animação inicial de felicidade (Glee inclui movimento de balanço)
        controller->getDisplay().setEyeMood(eEmotions::Glee);
    }

    // --- CONFIGURAÇÃO DE PERSONALIDADE ---
    // Configura o pool com PAUSAS NATURAIS longas (8s a 15s) e ativa o reset
    // visual para que o robô volte ao neutro entre as vinhetas.
    controller->getBehaviors().setPool(
        IDLE_POOL,
        GameConfig::IDLE_MIN_PAUSE_MS,
        GameConfig::IDLE_MAX_PAUSE_MS,
        true, // Loop ativado
        true  // ResetOnRest ativado para o modo Idle
    );

    // Primeiro lembrete visual após o atraso inicial configurado.
    _lastReminderTime = millis() + GameConfig::IDLE_INITIAL_DELAY_MS;
    _isPreparingReminder = false;
}

void IdleState::exit(GameController* controller)
{
    controller->getBehaviors().stop();
}

/** @section Atualização Lógica */

void IdleState::update(GameController* controller)
{
    DisplayOrchestrator& display = controller->getDisplay();
    BehaviorEngine& behaviors = controller->getBehaviors();
    unsigned long now = millis();

    // --- TIMEOUT PARA ECONOMIA DE ENERGIA ---
    // Se ninguém interagir com o robô pelo tempo de IDLE_TIMEOUT, ele se
    // desliga.
    if (now - controller->getStateStartTime() > GameConfig::IDLE_TIMEOUT_MS)
    {
        controller->shutdownSystem();
        return;
    }

    // Se houver uma instrução na tela, pausamos a lógica de lembrete.
    if (display.isInstructionActive())
    {
        return;
    }

    // LÓGICA DE LEMBRETE ORGÂNICA:
    // Só disparar a instrução se o tempo chegou e o robô está em repouso.
    if (now > _lastReminderTime && !behaviors.isInAction())
    {
        // Feedback sonoro do lembrete (BEW-WOO BEW-WOO)
        controller->getAudio().playFile(AudioFiles::IDLE_REMINDER);

        // O efeito "Zipper" do orquestrador cuidará da transição visual suave.
        display.showInstruction(
            Assets::ICON_FAUCET, GameConfig::INSTRUCTION_DISPLAY_MS
        );

        // Agenda o próximo lembrete no intervalo configurado.
        _lastReminderTime = now + GameConfig::IDLE_REMINDER_INTERVAL_MS;
    }
}

/** @section Tratamento de Eventos */

void IdleState::handleRFID(GameController* controller, const String& uid)
{
    if (uid == RFIDTags::FAUCET)
    {
        controller->changeState(RobotState::WET);
    }
    else
    {
        // Qualquer outra tag no IDLE é tratada como erro de sequência.
        controller->changeState(RobotState::ERROR);
    }
}
