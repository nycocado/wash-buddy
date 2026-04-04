# Wash Buddy

O **Wash Buddy** é um companheiro robótico interativo concebido para incentivar e gamificar os hábitos de higiene das crianças, transformando a lavagem das mãos ou o banho numa experiência visual e física divertida. Através de ecrãs expressivos, movimentos físicos e interação por cartões, o robô guia a criança ao longo dos passos recomendados para uma lavagem perfeita.

## Visão Geral do Sistema

O projeto original foi completamente reformulado para se focar numa experiência tátil e "standalone" (sem a necessidade de smartphones parentais ou conectividade Bluetooth para a sua operação base). O robô agora reage nativamente a *Tags* RFID, exibindo feedback emocional num ecrã OLED e movendo os seus braços e cabeça para interagir com o utilizador de forma síncrona com os ciclos de lavagem. O sistema de deteção por Strain Gauges e Giroscópio (MPU-6050) foi arquivado a favor de uma arquitetura mecânica direta (Servos) e de aproximação por rádio frequência.

O código foi reescrito sob uma arquitetura de C++ moderno, orientado a objetos, assente num padrão de motores e controladores fortemente tipados e altamente otimizados para microcontroladores.

## Funcionalidades Principais

* **Feedback Visual e Emocional (OLED):** Um ecrã OLED de 128x64 exibe expressões faciais fluidas a 50 FPS (via motor `ExpressionEngine`) em conjunto com um avançado motor de partículas *customizado* (`ParticleSystem`) que sobrepõe efeitos climáticos e físicos (chuva, bolhas de sabão dinâmicas, correntes de ar onduladas e confetes festivos rotativos em 3D) dependendo do estado atual da máquina de estados.
* **Movimento Físico Suavizado:** Três servomotores controlam os braços e a cabeça do robô através do `MotionController`. Em vez de deslocações abruptas, os movimentos utilizam algoritmos de *easing* logarítmico (suavização) calculados através de *Delta Time* (física independente da taxa de *frames*), tornando o robô orgânico e muito mais natural aos olhos de uma criança.
* **Interação Tátil por RFID:** O sistema utiliza um módulo leitor MFRC522 gerido pelo `RFIDReader`. A criança interage aproximando diferentes "fichas" (ex: Cartões ou porta-chaves encapsulados em formas de objetos de banho) que acionam estados ou comandos específicos do robô.
* **Áudio de Alta Fidelidade Sincronizado:** Integração com o hardware DFPlayer Pro (`AudioController`) para reprodução de playlists de áudio (MP3/WAV) específicas para cada etapa do ritual, proporcionando feedback sonoro imersivo e instruções claras.
* **Orquestração de Comportamento (Vignettes):** O robô utiliza o `BehaviorEngine` para executar pequenas cenas ou "vinhetas" de comportamento (combinações de olhar, expressão e gesto) de forma aleatória ou única. Isto evita repetições mecânicas e simula uma personalidade "viva", incluindo períodos de repouso (breathing space) entre as ações.
* **Gestão de Estados (State Machine):** Um controlador principal (`GameController`) orquestra as ações entre os motores e controladores mapeando um ciclo lógico estrito: `WET` (Molhar) -> `SOAP` (Ensaboar) -> `SCRUB` (Esfregar) -> `RINSE` (Enxaguar) -> `DRY` (Secar) -> `SUCCESS` (Sucesso).

## Arquitetura de Hardware

O projeto continua assente no ecossistema da framework Arduino utilizando um **ESP32** (suportando a variante C3 / DevKit C V4), e integra os seguintes componentes base atualizados:

* **Microcontrolador:** ESP32 (DevKit C V4 / HW-466AB)
* **Ecrã Principal:** Display OLED I2C 0.96" (Controlador SSD1306) no endereço `0x3C`
* **Leitor de Entrada:** Módulo RFID RC522 (Interface SPI Rápida)
* **Atuadores de Corpo:** 3x Micro Servos SG90 ligados através de pinos com suporte a hardware PWM.
* **Sistema de Alimentação:** Bateria LiPo com módulo de carregamento.

*(Pode visualizar a montagem exata dos pinos no simulador interativo abrindo o ficheiro `diagram.json`)*.

## Arquitetura de Software (Firmware)

O projeto usa **PlatformIO** em vez do clássico Arduino IDE. O *core* foi desenhado numa abordagem de **Padrão de Opções (Options Pattern)**, promovendo alto encapsulamento, segurança de memória e a eliminação total de *Magic Numbers* e macros *#define* inseguras na lógica.

Os subsistemas foram divididos nas seguintes bibliotecas dedicadas:

* `AudioController`: Gerencia o hardware DFPlayer Pro com fila de comandos assíncrona.
* `DisplayOrchestrator`: Atua como uma classe *Facade* que esconde e orquestra a complexidade visual, sincronizando o motor de expressões com o sistema de partículas.
* `MotionController`: Controlador físico que lida com a interpolação matemática dos motores, garantindo movimentos naturais e seguros dentro dos limites angulares.
* `ParticleSystem`: Um motor de física leve e polimórfico que gere o ciclo de vida de efeitos visuais (Bolhas, Chuva, Vento e Confetes).
* `ExpressionEngine`: Motor de renderização e comportamento que dá "vida" ao robô, controlando emoções, piscadas e a direção do olhar.
* `RFIDReader`: Isolador do hardware SPI que abstrai a complexidade do sensor MFRC522 para detecções simples de cartões e leitura de UIDs.
* `PowerController`: Gerencia o ciclo de vida de energia, lidando com o desligamento físico via hardware.
* `BehaviorEngine`: Motor de alto nível que gere o "pool" de comportamentos do robô, permitindo a execução de sequências expressivas e motoras síncronas com tempos de pausa configuráveis para um aspeto mais natural.

## Estrutura Atualizada do Repositório

```text
.
├── src/
│   └── iot/                # Diretório Raiz do Firmware Atual (Projeto PlatformIO)
│      ├── include/         # Cabeçalhos globais e enumerações semânticas (ex: RobotState.h)
│      ├── lib/             # Motores e Controladores (ex: BehaviorEngine, ParticleSystem)
│      ├── sounds/          # Arquivos de áudio otimizados para o hardware (WAV)
│      ├── src/             # Orquestrador Central (main.cpp e GameController)
│      ├── test/            # Suíte de testes unitários (Lógica, FSM e Sanidade de Tags)
│      ├── platformio.ini   # Configurações de compilação, otimização (LTO/O3) e dependências
│      ├── .clang-format    # Configurações de formatação de código para consistência
├── docs/                   # Relatórios e documentação técnica
└── README.md               # Visão Geral do Projeto
```

## Testes e Validação

O projeto utiliza o framework **Unity** para garantir a integridade do sistema. A suíte de testes cobre desde a matemática de suavização dos servos até testes de sanidade de configuração (como garantir que não existam UIDs de RFID duplicados), evitando falhas lógicas críticas durante a operação.

## Equipa

* Nycolas Souza
* Luan Ribeiro
* Lohanne Guedes
* Kira Sousa

---
*Projeto desenvolvido para a disciplina de Project Factory - 2026*
