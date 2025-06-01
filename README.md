# 🎮 Projeto: Prática 01 no RP2040 utilizando PIO

## 📋 Descrição

Este projeto demonstra a implementação de duas animações distintas em uma matriz de LEDs controladas pelo microcontrolador RP2040. Cada animação é acionada por um botão específico (Botão A e Botão B), utilizando interrupções para detecção de eventos e técnicas de debounce para garantir a precisão na leitura dos botões. O periférico PIO (Programmable Input/Output) do RP2040 é empregado para gerar os sinais de controle necessários para os LEDs.

## 🎯 Objetivos

* Implementar animações distintas acionadas por Botão A e Botão B.
* Utilizar interrupções para detecção eficiente dos eventos dos botões.
* Aplicar técnicas de debounce para evitar múltiplas detecções de um único acionamento.
* Empregar o periférico PIO para controle preciso da matriz de LEDs.

## 🛠️ Tecnologias Utilizadas

* Microcontrolador RP2040 (Raspberry Pi Pico W)
* Linguagem de Programação: C/C++
* Periférico PIO para controle de LEDs
* Interrupções e tratamento de debounce

## 📂 Estrutura do Projeto

* `pio_matrix.c`: Código principal que configura os botões, interrupções e inicializa as animações.
* `pio_matrix.pio`: Programa PIO responsável por gerar os sinais para os LEDs.
* `README.md`: Este arquivo de documentação.

## 🎥 Demonstração

Confira o vídeo demonstrativo do funcionamento do projeto:
https://youtube.com/shorts/98CmDunN1Ag?si=BhSqyGx-zQA60b7j
