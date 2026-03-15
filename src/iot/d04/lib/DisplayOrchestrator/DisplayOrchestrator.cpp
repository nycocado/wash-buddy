#include "DisplayOrchestrator.h"

// Referência externa para a instância global da biblioteca u8g2
// Esta instância é compartilhada entre os motores de desenho para evitar
// conflitos de memória e garantir renderização atômica.
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

DisplayOrchestrator::DisplayOrchestrator(const DisplayConfig& config)
    : _config(config), _eyes(), // Motor de expressões e comportamento facial
      _animations(u8g2), // Sistema de partículas vinculado ao buffer da u8g2
      _lastUpdate(0)
{
}

void DisplayOrchestrator::init()
{
    // Inicializa a lógica dos olhos usando as dimensões do orquestrador
    _eyes.begin(_config.screenWidth, _config.screenHeight);
    _eyes.setVisibility(true);

    Serial.println(F("[DISPLAY] Orquestrador visual inicializado com sucesso.")
    );
}

void DisplayOrchestrator::update()
{
    unsigned long now = millis();

    // Controle de cadência (FPS throttling)
    // Impede que o display tente atualizar mais rápido que o frameInterval
    // definido, economizando ciclos de processamento no Core 0.
    if (now - _lastUpdate < _config.frameInterval)
    {
        return;
    }

    float deltaTime = (now - _lastUpdate) / 1000.0f;
    _lastUpdate = now;

    // --- PIPELINE DE RENDERIZAÇÃO (Buffer Único) ---
    // Este fluxo garante que não haja flickering. Desenhamos todos os elementos
    // na RAM (u8g2 buffer) e enviamos o quadro pronto via I2C de uma só vez.

    // 1. Limpa o buffer de desenho para o novo quadro
    u8g2.clearBuffer();

    // 2. Processa lógicas de animação facial e desenha no buffer
    _eyes.update();
    _eyes.draw();

    // 3. Atualiza partículas atmosféricas e sobrepõe ao quadro atual
    _animations.update(deltaTime);
    _animations.draw();

    // 4. Transmite o buffer consolidado (Expressões + Partículas) para o OLED
    u8g2.sendBuffer();
}

/**
 * @section Comandos de Expressão Facial
 * Métodos que delegam o controle de humor e olhar para o ExpressionEngine.
 */

void DisplayOrchestrator::setEyeMood(eEmotions mood) { _eyes.setMood(mood); }

void DisplayOrchestrator::setEyeIdleMode(bool active)
{
    _eyes.setIdleMode(active);
}

void DisplayOrchestrator::blinkEyes() { _eyes.blink(); }

void DisplayOrchestrator::lookAt(float x, float y) { _eyes.lookAt(x, y); }

void DisplayOrchestrator::playConfused() { _eyes.playConfused(); }

void DisplayOrchestrator::playHappy() { _eyes.playHappy(); }

void DisplayOrchestrator::setEyesVisible(bool visible)
{
    _eyes.setVisibility(visible);
}

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

void DisplayOrchestrator::setMaxParticles(int max)
{
    _animations.setMaxParticles(max);
}

/**
 * @section Controle de Baixo Nível
 * Permite controle manual do buffer de desenho e gerenciamento de energia do
 * display.
 */

void DisplayOrchestrator::render() { u8g2.sendBuffer(); }

void DisplayOrchestrator::clear() { u8g2.clearBuffer(); }

/**
 * @section Gerenciamento de Energia
 * Permite colocar o display em modo de economia de energia (desligado) ou
 * acordá-lo.
 */

void DisplayOrchestrator::prepareForSleep()
{
    u8g2.setPowerSave(1);
    Serial.println(F("[DISPLAY] Driver em modo economia (Display OFF)."));
}

void DisplayOrchestrator::wakeUp()
{
    u8g2.setPowerSave(0);
    Serial.println(F("[DISPLAY] Driver acordado (Display ON)."));
}
