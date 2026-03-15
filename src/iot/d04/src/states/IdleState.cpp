#include "states/IdleState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

void IdleState::enter(GameController* controller)
{
    // Ativa o modo de "olhar ao redor" automático do display
    controller->getDisplay().setEyeIdleMode(true);
    _idleNextActionTime = millis() + 1000;
    _lastAction = -1;
    _actionStreak = 0;
}

void IdleState::update(GameController* controller)
{
    MotionController& motion = controller->getMotion();
    DisplayOrchestrator& display = controller->getDisplay();

    // Verifica se há alguma animação física em curso
    bool isCurrentlyAnimating = motion.isHeadAnimActive() ||
                                motion.isArmLAnimActive() ||
                                motion.isArmRAnimActive();

    // Se terminou uma animação agora, volta para o olhar normal e agenda a
    // próxima
    if (_wasAnimating && !isCurrentlyAnimating)
    {
        display.setEyeMood(eEmotions::Normal);
        _idleNextActionTime = millis() + random(3000, 8000);
    }
    _wasAnimating = isCurrentlyAnimating;

    // Se estiver animando, apenas espera concluir
    if (isCurrentlyAnimating)
    {
        return;
    }

    // Movimento sutil de "respiração" nos braços quando parado
    float breatheOffset = (sin(millis() / 500.0f) + 1.0f) * 3.0f;
    motion.moveArmL(breatheOffset, 1.0f);
    motion.moveArmR(breatheOffset, 1.0f);

    // Lógica para disparar uma nova ação aleatória baseada no tempo
    if (millis() > _idleNextActionTime)
    {
        int nextAction = random(0, 3);

        // Evita que o robô faça a mesma coisa muitas vezes seguidas
        if (nextAction == _lastAction)
        {
            _actionStreak++;
            if (_actionStreak >= 2)
            {
                nextAction = (nextAction + 1) % 3;
                _actionStreak = 1;
            }
        }
        else
        {
            _actionStreak = 1;
        }

        _lastAction = nextAction;

        // Escolha da Ação Aleatória
        if (nextAction == 0) // Olhar ao redor (Focado)
        {
            display.setEyeMood(eEmotions::Focused);
            motion.playHeadChoreography(
                ChoreographyLibrary::idlePanLook(), 0.0f, false
            );
        }
        else if (nextAction == 1) // Acenar (Alegre)
        {
            display.setEyeMood(eEmotions::Glee);
            bool waveLeft = (random(0, 2) == 0);
            if (waveLeft)
            {
                motion.playArmLChoreography(
                    ChoreographyLibrary::waveHand(), 0.0f, false
                );
            }
            else
            {
                motion.playArmRChoreography(
                    ChoreographyLibrary::waveHand(), 0.0f, false
                );
            }
        }
        else // Olhar lateral rápido (Surpreso)
        {
            display.setEyeMood(eEmotions::Surprised);
            int glanceAngle = (random(0, 2) == 0) ? 70 : 110;
            motion.playHeadChoreography(
                ChoreographyLibrary::lookSide(glanceAngle), 0.0f, false
            );
        }
    }
}

void IdleState::exit(GameController* controller) {}

void IdleState::handleRFID(GameController* controller, const String& uid)
{
    // O ritual deve OBRIGATORIAMENTE começar pela torneira
    if (uid == RFIDTags::FAUCET)
    {
        controller->changeState(RobotState::WET);
    }
    else
    {
        // Se a criança tentar outro objeto antes da torneira, sinaliza erro
        controller->changeState(RobotState::ERROR);
    }
}
