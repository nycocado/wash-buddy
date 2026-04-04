#pragma once

#include "BubbleParticle.h"
#include "ConfettiParticle.h"
#include "Particle.h"
#include "RainParticle.h"
#include "SparkleParticle.h"
#include "WindParticle.h"
#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @brief Tipos de efeitos visuais de partículas disponíveis.
 */
enum class EffectType
{
    NONE,       ///< Sem efeito
    BUBBLES,    ///< Bolhas subindo
    RAIN_LIGHT, ///< Chuva fina
    RAIN_HEAVY, ///< Chuva forte
    CONFETTI,   ///< Explosão de confetes
    WIND,       ///< Linhas horizontais
    SPARKLE     ///< Brilhos de sucesso
};

/**
 * @brief Configurações globais do sistema de partículas.
 */
struct ParticleSystemConfig
{
        int activeLimit = 30;  ///< Limite máximo de partículas simultâneas
        int spawnChance = 15;  ///< Probabilidade (0-100) de geração por frame
        int screenWidth = 128; ///< Largura da área de spawn
        int screenHeight = 64; ///< Altura da área de spawn
};

/**
 * @class ParticleSystem
 * @brief Motor de efeitos especiais baseado em partículas.
 *
 * Gerencia o ciclo de vida de diversos tipos de partículas que decoram o
 * display.
 */
class ParticleSystem
{
    public:
        /**
         * @brief Construtor do sistema.
         * @param display Referência para a U8g2.
         * @param config Definições globais de limites.
         */
        ParticleSystem(
            U8G2& display,
            const ParticleSystemConfig& config = ParticleSystemConfig()
        );

        /** @brief Destrutor para limpeza de memória. */
        ~ParticleSystem();

        /**
         * @brief Altera o efeito atmosférico ativo.
         * @param type Novo tipo de efeito.
         */
        void setEffect(EffectType type);

        /**
         * @brief Atualiza a física e ciclo de vida de todas as partículas.
         * @param deltaTime Tempo decorrido (segundos).
         */
        void update(float deltaTime);

        /** @brief Renderiza todas as partículas ativas. */
        void draw();

        /** @brief Define a chance de geração de novas partículas. */
        void setSpawnChance(int chance) { _config.spawnChance = chance; }

        /** @brief Define o limite máximo de partículas vivas. */
        void setMaxParticles(int max)
        {
            _config.activeLimit = (max > MAX_PARTICLES) ? MAX_PARTICLES : max;
        }

    private:
        U8G2& _display;
        ParticleSystemConfig _config;
        static constexpr int MAX_PARTICLES = 40;
        Particle* _particles[MAX_PARTICLES];
        EffectType _currentEffect;

        BubbleSettings _bubbleConfig;
        RainSettings _rainConfig;
        SparkleSettings _sparkleConfig;
        ConfettiSettings _confettiConfig;
        WindSettings _windConfig;

        /** @brief Instancia uma nova partícula no slot especificado. */
        void spawnParticle(int index);

        /** @brief Remove e deleta todas as partículas. */
        void clearAll();
};
