#include "DisplayOrchestrator.h"

// Referência externa para a instância global da biblioteca u8g2
// Esta instância é compartilhada entre os motores de desenho para evitar
// conflitos de memória e garantir renderização atômica.
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

DisplayOrchestrator::DisplayOrchestrator(const DisplayConfig& config)
    : _config(config), _eyes(), // Motor de expressões e comportamento facial
      _animations(u8g2), // Sistema de partículas vinculado ao buffer da u8g2
      _lastUpdate(0), _debugText("")
{
}

void DisplayOrchestrator::init()
{
    // Inicializa a lógica dos olhos usando as dimensões do orquestrador
    _eyes.begin(_config.screenWidth, _config.screenHeight);

    Serial.println(F("[DISPLAY] Orquestrador visual inicializado com sucesso.")
    );
}

void DisplayOrchestrator::update(float deltaTime)
{
    unsigned long now = millis();

    // --- LÓGICA DE TRANSIÇÃO PROGRESSIVA (CORTINAS) ---
    // Gerencia o fechamento e abertura da 'cortina' entre olhos e ícones
    const float transitionSpeed = 5.0f; // Velocidade da transição

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

    // --- LÓGICA DE EXPIRAÇÃO DE ÍCONE ---
    if (_currentInstructionIcon != nullptr && _instructionEndTime > 0 &&
        now > _instructionEndTime && _transitionState == TransitionState::IDLE)
    {
        // Se o ícone expirar, inicia a transição para voltar aos olhos
        _pendingIcon = nullptr;
        _transitionState = TransitionState::CLOSING;
    }

    // 1. Limpa o buffer de desenho
    u8g2.clearBuffer();

    // 2. Desenha a base (Ícone de Instrução OU Olhos)
    if (_currentInstructionIcon != nullptr)
    {
        // Animação de Floating (apenas se não estiver abrindo/fechando)
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

    // 3. Efeito 'Zíper' de Transição (Cortinas)
    if (_transitionProgress > 0.0f)
    {
        int h = (int)(_transitionProgress * 32); // Metade da tela
        u8g2.setDrawColor(0);                    // Cor preta para apagar
        u8g2.drawBox(0, 0, 128, h);              // Cortina superior
        u8g2.drawBox(0, 64 - h, 128, h);         // Cortina inferior
        u8g2.setDrawColor(1);                    // Volta ao branco normal
    }

    // 4. Partículas (Sempre visíveis)
    _animations.update(deltaTime);
    _animations.draw();

    // ... restante do código de debug e renderização ...

    // 4. Debug Overlay
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

void DisplayOrchestrator::setInstructionIcon(
    const uint8_t* icon,
    uint8_t width,
    uint8_t height
)
{
    _pendingIcon = icon;
    _instructionWidth = (width == 0) ? _config.iconDefaultSize : width;
    _instructionHeight = (height == 0) ? _config.iconDefaultSize : height;
    _instructionEndTime = 0; // Sem expiração automática
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
 * @section Comandos de Expressão Facial
 */

void DisplayOrchestrator::setEyeMood(eEmotions mood)
{
    // Se mudarmos o humor e houver um ícone ativo, fechamos o zíper primeiro!
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
 * @section Gestão do Sistema de Partículas
 * Controla os efeitos atmosféricos que ambientam o ritual de lavagem.
 */

void DisplayOrchestrator::setParticleEffect(EffectType type)
{
    _animations.setEffect(type);
}

void DisplayOrchestrator::setParticleSpawnChance(int chance)
{
    _animations.setSpawnChance(chance);
}

void DisplayOrchestrator::setDebugText(const String& text)
{
    _debugText = text;
}

/**
 * @section Gerenciamento de Energia
 * Permite colocar o display em modo de economia de energia (desligado) ou
 * acordá-lo.
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

void DisplayOrchestrator::wakeUp()
{
    u8g2.setPowerSave(0);
    Serial.println(F("[DISPLAY] Driver acordado (Display ON)."));
}
