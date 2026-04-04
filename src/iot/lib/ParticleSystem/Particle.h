#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @class Particle
 * @brief Classe base abstrata para todos os tipos de partículas.
 *
 * Define a interface obrigatória e atributos fundamentais para efeitos visuais.
 */
class Particle
{
    public:
        /** @brief Destrutor virtual para limpeza das classes derivadas. */
        virtual ~Particle() {}

        /**
         * @brief Atualiza a física da partícula.
         * @param deltaTime Tempo decorrido (segundos).
         * @param screenWidth Largura da tela.
         * @param screenHeight Altura da tela.
         * @return true se a partícula continua viva, false se deve ser
         * destruída.
         */
        virtual bool
        update(float deltaTime, int screenWidth, int screenHeight) = 0;

        /**
         * @brief Renderiza a partícula no display.
         * @param display Referência para a biblioteca U8G2.
         */
        virtual void draw(U8G2& display) = 0;

    protected:
        float x;     ///< Posição horizontal atual.
        float y;     ///< Posição vertical atual.
        float speed; ///< Velocidade base (pixels/s).
};
