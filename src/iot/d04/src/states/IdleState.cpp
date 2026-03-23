#include "states/IdleState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"
#include "assets/Images.h"

// Define o Pool de Personalidade para o modo de repouso (Idle)
static const std::vector<BehaviorVignette> IDLE_POOL = {
    {eEmotions::Focused, 0.0f, 0.2f, ChoreographyLibrary::idlePanLook(), 5000},
    {eEmotions::Glee, 0.0f, 0.0f, ChoreographyLibrary::waveHand(), 4000},
    {eEmotions::Surprised, 0.7f, 0.0f, ChoreographyLibrary::lookSide(70), 3000},
    {eEmotions::Surprised,
     -0.7f,
     0.0f,
     ChoreographyLibrary::lookSide(110),
     3000}};

/** @section Ciclo de Vida */

void IdleState::enter(GameController* controller)
{
    // Configura o pool com PAUSAS NATURAIS longas (8s a 15s)
    controller->getBehaviors().setPool(
        IDLE_POOL, GameConfig::IDLE_MIN_PAUSE_MS, GameConfig::IDLE_MAX_PAUSE_MS
    );

    controller->getDisplay().setEyeIdleMode(true);

    // Primeiro lembrete após o atraso inicial configurado
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

    if (display.isInstructionActive())
    {
        return;
    }

    // LÓGICA DE LEMBRETE ORGÂNICA:
    // Só disparar a instrução se:
    // 1. O tempo de lembrete chegou
    // 2. O robô NÃO está executando uma ação (está no período de pausa/repouso)
    if (now > _lastReminderTime && !behaviors.isInAction())
    {
        // O efeito "Zipper" cuidará da transição visual suave.
        display.showInstruction(
            Assets::ICON_FAUCET, GameConfig::INSTRUCTION_DISPLAY_MS
        );

        // Próximo lembrete no intervalo configurado (30s)
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
        controller->changeState(RobotState::ERROR);
    }
}
