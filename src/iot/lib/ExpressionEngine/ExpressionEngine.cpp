#include "ExpressionEngine.h"

// Referência externa para a instância global da biblioteca u8g2.
// O motor de expressões utiliza esta instância para desenhar as formas
// geométricas básicas que compõem os olhos.
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

ExpressionEngine::ExpressionEngine() : _face(nullptr) {}

ExpressionEngine::~ExpressionEngine()
{
    if (_face)
    {
        delete _face;
    }
}

/**
 * @section Ciclo de Vida e Renderização
 */

void ExpressionEngine::begin(
    uint16_t width,
    uint16_t height,
    const ExpressionConfig& config
)
{
    // A classe Face inicializa o u8g2 internamente e configura o buffer.
    // O tamanho do olho define a escala de renderização no display.
    _face = new Face(width, height, config.eyeSize);

    // --- CONFIGURAÇÃO INICIAL (Behavior) ---
    // Ativamos piscadas aleatórias, mas mantemos o olhar fixo inicialmente.
    _face->RandomBlink = true;
    _face->Blink.Timer.SetIntervalMillis(config.blinkInterval);
    _face->RandomLook = true;
    _face->RandomBehavior = false;

    // Garante que o robô comece com uma expressão neutra
    _face->Expression.GoTo_Normal();

    // Limpa variações residuais (piscadas parciais ou tremores)
    _face->LeftEye.Variation1.Clear();
    _face->LeftEye.Variation2.Clear();
    _face->RightEye.Variation1.Clear();
    _face->RightEye.Variation2.Clear();
}

void ExpressionEngine::update()
{
    if (_face)
    {
        // --- PROCESSAMENTO DE COMPORTAMENTOS ---
        // Se o modo randômico estiver ativo, sorteia novas emoções.
        if (_face->RandomBehavior)
            _face->Behavior.Update();

        // Processa o movimento suave do olhar (LookAt ou RandomLook).
        if (_face->RandomLook)
            _face->Look.Update();

        // Processa o temporizador de piscada automática.
        if (_face->RandomBlink)
            _face->Blink.Update();
    }
}

void ExpressionEngine::draw()
{
    if (_face)
    {
        // --- CÁLCULO DE POSICIONAMENTO ---
        // Calcula o centro de cada olho baseado na distância interpupilar
        // e no tamanho base definido na configuração.

        // Olho Esquerdo
        _face->LeftEye.CenterX =
            _face->CenterX - _face->EyeSize / 2 - _face->EyeInterDistance;
        _face->LeftEye.CenterY = _face->CenterY;
        _face->LeftEye.Draw();

        // Olho Direito
        _face->RightEye.CenterX =
            _face->CenterX + _face->EyeSize / 2 + _face->EyeInterDistance;
        _face->RightEye.CenterY = _face->CenterY;
        _face->RightEye.Draw();
    }
}

/**
 * @section Gestão de Expressões e Comportamento
 */

void ExpressionEngine::setMood(eEmotions mood)
{
    if (_face)
    {
        // Altera o humor atual. A biblioteca processará a transição
        // de forma suave no próximo ciclo de desenho.
        _face->Behavior.GoToEmotion(mood);
    }
}

void ExpressionEngine::setIdleMode(bool active)
{
    if (_face)
    {
        // Ativa comportamentos que dão "vida" ao robô quando ele não
        // está realizando uma tarefa específica.
        _face->RandomLook = active;
        _face->RandomBlink = active;
    }
}

/**
 * @section Controles Direcionais e Presets
 */

void ExpressionEngine::blink()
{
    if (_face)
        _face->DoBlink();
}

void ExpressionEngine::lookAt(float x, float y)
{
    if (_face)
    {
        // Desativa o olhar aleatório para focar na coordenada pedida.
        _face->RandomLook = false;
        _face->Look.LookAt(x, y);
    }
}

void ExpressionEngine::playConfused()
{
    if (_face)
        _face->Expression.GoTo_Annoyed();
}

void ExpressionEngine::playHappy()
{
    if (_face)
        _face->Expression.GoTo_Happy();
}
