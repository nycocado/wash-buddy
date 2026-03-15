#pragma once

#include "Particle.h"

/**
 * @struct SparkleSettings
 * @brief Configurações para o efeito visual de Brilho/Cintilação.
 *
 * Controla os limites de velocidade de subida e a frequência da
 * animação de piscagem das estrelas de brilho.
 */
struct SparkleSettings
{
        float minSpeed = 5.0f;  ///< Velocidade mínima de subida lenta.
        float maxSpeed = 15.0f; ///< Velocidade máxima de subida lenta.
        float blinkSpeed = 10.0f; ///< Frequência angular da cintilação (rad/s).
};

/**
 * @class SparkleParticle
 * @brief Representa uma partícula de brilho que pisca e sobe lentamente.
 *
 * Esta partícula é ideal para efeitos de "limpeza" ou "sucesso".
 * Ela simula o brilho de estrelas através de uma oscilação senoidal do seu
 * tamanho aparente enquanto flutua suavemente para cima.
 */
class SparkleParticle : public Particle
{
    public:
        /**
         * @brief Construtor da partícula de brilho.
         *
         * Inicializa a fase de cintilação e a velocidade de subida de forma
         * aleatória.
         *
         * @param startX Posição horizontal inicial.
         * @param startY Posição vertical inicial.
         * @param cfg Referência para as configurações de brilho.
         */
        SparkleParticle(float startX, float startY, const SparkleSettings& cfg);

        /**
         * @brief Atualiza a lógica de cintilação e movimento vertical.
         *
         * Incrementa a fase da senoide de brilho e desloca a partícula para
         * cima.
         *
         * @param deltaTime Tempo decorrido desde o último quadro (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return true se a partícula ainda deve ser desenhada (não saiu da
         * tela).
         */
        bool
        update(float deltaTime, int screenWidth, int screenHeight) override;

        /**
         * @brief Renderiza a estrela de brilho com tamanho variável.
         *
         * Utiliza a fase de cintilação para determinar quantos pixels compõem
         * a forma de "cruz" da estrela no quadro atual.
         *
         * @param display Referência para o controle do display U8G2.
         */
        void draw(U8G2& display) override;

    private:
        float blinkTimer; ///< Fase acumulada para o cálculo do brilho (seno).
        const SparkleSettings& _cfg; ///< Referência para configurações.
};
