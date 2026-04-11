# Wash Buddy

O **Wash Buddy** é um companheiro robótico interativo concebido para incentivar e gamificar os hábitos de higiene das crianças, transformando a lavagem das mãos ou o banho numa experiência visual e física divertida. Através de expressões faciais fluidas, movimentos orgânicos e interação por rádio frequência (RFID), o robô guia a criança ao longo dos passos recomendados para uma higienização perfeita.

## Visão Geral do Sistema

O projeto evoluiu de um conceito inicial baseado em sensores de pressão para uma experiência **standalone** e imersiva. O robô reage nativamente a *Tags* RFID, exibindo feedback emocional num ecrã OLED e movendo os seus braços e cabeça de forma síncrona com os ciclos de lavagem.

Esta versão final foi desenvolvida em estreita colaboração com a equipa de Design **D04**, resultando numa integração harmoniosa entre o hardware eletrónico e o artefacto físico.

## Funcionalidades Principais

* **Feedback Visual e Emocional (OLED):** Expressões faciais a 50 FPS processadas em tempo real, com um motor de partículas customizado para efeitos de bolhas, chuva e confetes.
* **Movimentação Orgânica Suavizada:** Três servomotores controlados com algoritmos de *easing* logarítmico, garantindo movimentos naturais e seguros.
* **Interação Tátil por RFID:** Interface segura e à prova de água através da leitura de objetos lúdicos (sabão, esponja, toalha) encapsulando tags RFID.
* **Áudio de Alta Fidelidade Sincronizado:** Instruções de voz e feedback sonoro via DFPlayer Pro, mapeados para cada etapa do ritual de higiene.
* **Gestão de Energia Inteligente:** Circuito de auto-desligamento físico via MOSFET para preservação da bateria (4400mAh) após a conclusão do ciclo ou inatividade prolongada.

## Arquitetura de Software (Firmware)

O firmware foi desenhado sob uma arquitetura modular em **C++ Moderno (PlatformIO)**, separando a abstração de hardware da camada de interface e comportamento.

### Padrões de Projeto e Multiprocessamento

* **State & Options Patterns:** O fluxo é gerido por uma Máquina de Estados Finita (FSM) altamente tipada, eliminando condicionais complexas e facilitando a expansão do sistema.
* **FreeRTOS (Dual-Core):** O **Core 0** do ESP32 é dedicado exclusivamente à renderização gráfica pesada, enquanto o **Core 1** processa a lógica de controlo, áudio e interpolação dos motores, garantindo uma interface fluida a 50 FPS.

## Arquitetura de Hardware

A eletrónica foi consolidada numa **PCB customizada de dois andares (Stacked PCB)** para isolar o ruído elétrico dos motores dos barramentos de comunicação sensíveis:

* **Módulo Superior (Lógica):** ESP32-WROOM-32, OLED 0.96", RFID RC522 e DFPlayer Pro.
* **Módulo Inferior (Potência):** Carregador DFR1026, MOSFETs de isolamento e filtragem de ruído.

### Principais Componentes (BOM D04)

| Componente | Função |
| :--- | :--- |
| **ESP32-WROOM-32** | Microcontrolador Principal |
| **OLED 0.96" I2C** | Interface Visual (Expressões) |
| **RC522 RFID** | Sensor de Entrada (Tags 13.56MHz) |
| **DFPlayer Pro** | Controlador de Áudio + 2x Colunas 2W |
| **3x Servos SG90** | Atuadores de Cabeça e Braços |
| **Bateria 4400mAh** | Alimentação LiPo (2x 2200mAh) |

## Estrutura do Repositório

```text
.
├── src/
│   └── iot/            # Código-fonte do Firmware (Projeto PlatformIO)
│      ├── include/     # Cabeçalhos globais e enumerações semânticas
│      ├── lib/         # Motores e Controladores (Behavior, Particles, Expression)
│      ├── src/         # Orquestrador Central (main.cpp e GameController)
│      └── test/        # Suíte de testes unitários (Unity)
├── docs/               # Relatórios (Milestone 1/2), BOMs e Esquemas Elétricos
├── media/              # Assets visuais, vídeos e biblioteca de sons
└── README.md           # Visão Geral do Projeto
```

## Equipa e Responsabilidades

* **Nycolas Souza:** Desenvolvimento integral do firmware (C++/PlatformIO), arquitetura OO, paralelização via FreeRTOS e coreografia matemática dos servos.
* **Luan Ribeiro:** Engenharia de hardware e desenho do diagrama elétrico. Projetou o isolamento com MOSFETs e o circuito de auto-shutdown.
* **Lohanne Guedes:** Prototipagem física e integração de laboratório. Responsável pela soldadura integral da PCB e montagem no chassi.
* **Kira Sousa:** Desenho do sistema lógico, mapeamento das regras de negócio (FSM) e Sound Design (curadoria de áudio).

## Anexos e Documentação Técnica

1. **Código Fonte:** [src/iot/](src/iot/)
2. **Esquema Elétrico (Final - D04):** [docs/milestone-2/circuit-d04.pdf](docs/milestone-2/circuit-d04.pdf)
3. **Bill of Materials (BOM - D04):** [docs/milestone-2/bom-d04.xlsx](docs/milestone-2/bom-d04.xlsx)
4. **Relatório Técnico (Milestone 2):** [docs/milestone-2/report.pdf](docs/milestone-2/report.pdf)
5. **Vídeo de Demonstração:** [media/milestone-2/video.mp4](media/milestone-2/video.mp4)
6. **Manual de Montagem:** [media/milestone-2/assembly-manual.png](media/milestone-2/assembly-manual.png)
7. **Modelo Físico:** [media/milestone-2/physical-model.jpeg](media/milestone-2/physical-model.jpeg)
8. **Arquivos de Áudio:** [media/sounds/](media/sounds/)

---
*Projeto desenvolvido para a disciplina de Project Factory - 2026*
