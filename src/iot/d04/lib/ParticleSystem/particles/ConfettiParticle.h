#pragma once

#include "Particle.h"

/**
 * @struct ConfettiSettings
 * @brief Configurações para o comportamento físico do efeito de Confete.
 *
 * Define os parâmetros de lançamento, gravidade e dissipação de energia
 * (resistência do ar) para as partículas de confete.
 */
struct ConfettiSettings
{
        float minSpeed = 40.0f;  ///< Velocidade inicial mínima de lançamento.
        float maxSpeed = 100.0f; ///< Velocidade inicial máxima de lançamento.
        float gravity = 30.0f;   ///< Aceleração da gravidade (pixels/s^2).
        float airResistance =
            0.98f; ///< Coeficiente de amortecimento da velocidade (0 a 1).
};

/**
 * @class ConfettiParticle
 * @brief Representa uma partícula de confete com física de projétil e rotação.
 *
 * Esta partícula simula um confete lançado ao ar, sofrendo ação da gravidade,
 * resistência do ar e exibindo uma rotação visual (efeito de "flicker")
 * que simula a queda de um papel retangular em 3D.
 */
class ConfettiParticle : public Particle
{
    public:
        /**
         * @brief Construtor da partícula de confete.
         *
         * Define uma trajetória inicial aleatória em um círculo completo (360
         * graus) e inicializa os parâmetros de rotação visual.
         *
         * @param startX Posição X inicial de lançamento.
         * @param startY Posição Y inicial de lançamento.
         * @param cfg Referência para as configurações de física do confete.
         */
        ConfettiParticle(
            float startX,
            float startY,
            const ConfettiSettings& cfg
        );

        /**
         * @brief Atualiza a física (posição, velocidade, rotação) do confete.
         *
         * Aplica integração de Euler para posição, gravidade para velocidade
         * vertical, e fator de resistência do ar para ambos os eixos.
         *
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @param screenWidth Largura da tela para verificação de limites.
         * @param screenHeight Altura da tela para verificação de limites.
         * @return true se a partícula ainda está dentro dos limites ativos da
         * simulação.
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /**
         * @brief Renderiza o confete no display.
         *
         * Desenha um pequeno retângulo cuja largura oscila para simular uma
         * rotação em torno do eixo vertical (efeito 3D simplificado).
         *
         * @param display Referência para o controle do display U8G2.
         */
        void draw(U8G2& display) override;

    private:
        float vx, vy;    ///< Componentes vetoriais da velocidade (pixels/s).
        float size;      ///< Dimensão base (lado) do confete.
        float spinSpeed; ///< Velocidade angular da rotação visual.
        float spinPhase; ///< Fase atual da oscilação de largura.
        const ConfettiSettings& _cfg; ///< Referência para configurações.
};
