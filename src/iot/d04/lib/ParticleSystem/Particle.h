#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @class Particle
 * @brief Classe base abstrata para todos os tipos de partículas no sistema.
 *
 * Esta classe define a interface obrigatória e os atributos fundamentais
 * (posição e velocidade) para qualquer efeito visual de partícula. Através do
 * polimorfismo, o ParticleSystem pode gerenciar diferentes tipos de partículas
 * de forma genérica.
 */
class Particle
{
    public:
        /**
         * @brief Destrutor virtual para garantir a limpeza correta das classes
         * derivadas.
         */
        virtual ~Particle() {}

        /**
         * @brief Atualiza a lógica/física da partícula.
         * @param deltaTime Tempo decorrido desde o último quadro (em segundos).
         * @param screenWidth Largura da área de renderização.
         * @param screenHeight Altura da área de renderização.
         * @return true se a partícula ainda deve continuar viva, false se deve
         * ser destruída.
         */
        virtual bool
        update(float deltaTime, int screenWidth, int screenHeight) = 0;

        /**
         * @brief Renderiza a partícula no buffer do display.
         * @param display Referência para a instância da biblioteca U8G2.
         */
        virtual void draw(U8G2& display) = 0;

    protected:
        float
            x; ///< Posição horizontal atual no sistema de coordenadas da tela.
        float y; ///< Posição vertical atual no sistema de coordenadas da tela.
        float speed; ///< Velocidade base de deslocamento (pixels por segundo).
};
