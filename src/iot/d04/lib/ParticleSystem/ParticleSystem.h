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
 * @brief Tipos de efeitos visuais de partículas disponíveis no sistema.
 */
enum class EffectType
{
    NONE,       ///< Nenhum efeito ativo
    BUBBLES,    ///< Bolhas subindo (ideal para sabão/limpeza)
    RAIN_LIGHT, ///< Chuva fina caindo
    RAIN_HEAVY, ///< Chuva forte caindo
    CONFETTI,   ///< Explosão de confetes (comemoração)
    WIND,       ///< Linhas de vento horizontais
    SPARKLE     ///< Brilhos de limpeza/sucesso
};

/**
 * @brief Configurações globais do sistema de partículas.
 */
struct ParticleSystemConfig
{
        int activeLimit = 30; ///< Limite máximo de partículas simultâneas
        int spawnChance =
            15; ///< Probabilidade (0-100) de gerar nova partícula por frame
        int screenWidth = 128; ///< Largura da área de spawn
        int screenHeight = 64; ///< Altura da área de spawn
};

/**
 * @class ParticleSystem
 * @brief Motor de efeitos especiais baseado em sistemas de partículas.
 *
 * Esta classe gerencia o ciclo de vida (criação, atualização e destruição) de
 * diversos tipos de partículas que decoram o display OLED. Cada efeito possui
 * sua própria física e comportamento visual.
 */
class ParticleSystem
{
    public:
        /**
         * @brief Construtor do sistema de partículas.
         * @param display Referência para a instância da U8g2.
         * @param config Definições globais de limites e área.
         */
        ParticleSystem(
            U8G2& display,
            const ParticleSystemConfig& config = ParticleSystemConfig()
        );
        ~ParticleSystem();

        /**
         * @brief Altera o efeito atmosférico atual.
         * @param type Novo tipo de efeito.
         */
        void setEffect(EffectType type);

        /**
         * @brief Atualiza a física de todas as partículas ativas.
         * @param deltaTime Tempo decorrido desde o último quadro (s).
         */
        void update(float deltaTime);

        /**
         * @brief Renderiza todas as partículas no buffer da U8g2.
         */
        void draw();

        // Acesso às configurações específicas de cada efeito
        BubbleSettings& getBubbleSettings() { return _bubbleConfig; }
        RainSettings& getRainSettings() { return _rainConfig; }
        SparkleSettings& getSparkleConfig() { return _sparkleConfig; }
        ConfettiSettings& getConfettiConfig() { return _confettiConfig; }
        WindSettings& getWindSettings() { return _windConfig; }

        /**
         * @brief Define a chance de spawn de novas partículas.
         */
        void setSpawnChance(int chance) { _config.spawnChance = chance; }

        /**
         * @brief Define o limite máximo de partículas.
         */
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

        // Configurações persistentes por tipo de efeito
        BubbleSettings _bubbleConfig;
        RainSettings _rainConfig;
        SparkleSettings _sparkleConfig;
        ConfettiSettings _confettiConfig;
        WindSettings _windConfig;

        /**
         * @brief Cria uma nova partícula em um slot vazio.
         */
        void spawnParticle(int index);

        /**
         * @brief Remove e deleta todas as partículas ativas.
         */
        void clearAll();
};
