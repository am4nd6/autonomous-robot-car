# Carrinho Robô Autônomo com VL53L0X e HC-SR04

Projeto de um carrinho robô capaz de se movimentar autonomamente e escolher caminhos com base na distância dos obstáculos. O sistema utiliza um sensor de distância a laser por tempo de voo (`VL53L0X`) montado em um servo para observar a frente e as laterais, além de um sensor ultrassônico (`HC-SR04`) voltado para trás.

Este guia foi escrito para que uma pessoa sem experiência prévia consiga identificar os componentes, montar as ligações, instalar o software, carregar o programa e realizar os primeiros testes com segurança.

> **Importante:** o repositório contém o programa e a pinagem usada pelo código, mas não especifica o modelo exato do Arduino, do driver de motores, dos motores ou da bateria. Antes de ligar a alimentação, confira o manual do seu hardware e confirme a pinagem.

## Sumário

- [O que o projeto faz](#o-que-o-projeto-faz)
- [Componentes necessários](#componentes-necessários)
- [Como o sistema funciona](#como-o-sistema-funciona)
- [Pinagem](#pinagem)
- [Montagem e alimentação](#montagem-e-alimentação)
- [Instalação do software](#instalação-do-software)
- [Upload do programa](#upload-do-programa)
- [Primeiro teste](#primeiro-teste)
- [Operação e monitor serial](#operação-e-monitor-serial)
- [Configurações ajustáveis](#configurações-ajustáveis)
- [Solução de problemas](#solução-de-problemas)
- [Limitações e melhorias](#limitações-e-melhorias)
- [Estrutura do repositório](#estrutura-do-repositório)
- [Licença](#licença)

## O que o projeto faz

Quando ligado, o Arduino:

1. Inicializa o servo e o sensor `VL53L0X`.
2. Mede as distâncias à frente, à direita, à esquerda e atrás.
3. Escolhe a direção com maior espaço disponível quando o caminho da frente está bloqueado.
4. Aciona o driver para mover os motores.
5. Enquanto avança, verifica continuamente a frente e as regiões diagonais.
6. Para ao detectar um obstáculo próximo e faz uma nova avaliação do ambiente.

As distâncias usadas pelo programa são expressas em centímetros. O `VL53L0X` mede em milímetros e o código converte o resultado para centímetros; o `HC-SR04` já é convertido diretamente para centímetros.

## Componentes necessários

- Uma placa Arduino compatível com o sketch, como Arduino Uno ou Nano.
- Um sensor de tempo de voo `VL53L0X` compatível com a biblioteca Adafruit.
- Um sensor ultrassônico `HC-SR04`.
- Um servo motor para girar o `VL53L0X`.
- Um driver de motores de dois canais. O modelo não é definido neste projeto.
- Dois motores DC com rodas.
- Chassi, roda livre/castor, parafusos e suportes.
- Bateria ou fonte adequada para os motores.
- Cabos jumper e, se necessário, protoboard.
- Cabo USB para programar o Arduino.

## Como o sistema funciona

### Sensor VL53L0X

O `VL53L0X` emite luz infravermelha e calcula o tempo necessário para o sinal retornar. Ele é instalado sobre um servo e apontado para diferentes ângulos:

| Direcao | Angulo aproximado do servo |
|---|---:|
| Direita | 10 graus |
| Direita 2 | 45 graus |
| Frente | 85 graus |
| Esquerda 2 | 115 graus |
| Esquerda | 150 graus |

Os valores de `DIREITA2` e `ESQUERDA2` são usados durante o deslocamento. A leitura completa das direções usa frente, direita, esquerda e traseira.

### Sensor HC-SR04

O `HC-SR04` fica voltado para trás. O Arduino envia um pulso pelo pino `TRIG`, mede o tempo de retorno no pino `ECHO` e calcula a distância. O sensor precisa estar corretamente alinhado e não deve receber tensão incompatível com a placa.

### Decisão de movimento

- `distancia_minima = 30`: abaixo de 30 cm, a direção é considerada perigosa.
- `distancia_para_andar = 50`: uma direção é considerada com espaço suficiente a partir de 50 cm.
- Se a frente estiver livre, o carrinho segue em frente.
- Se a frente estiver bloqueada, o programa compara os lados e a traseira.
- Se houver empate ou uma situação não prevista, a direção escolhida pode depender dos valores lidos e do último movimento.

## Pinagem

As ligações abaixo são as utilizadas pelo arquivo `Code/versao4_carrinho.ino`.

### Driver de motores

| Arduino | Nome no código | Função |
|---:|---|---|
| D3 | `entrada1` | Controle do motor/canal 1, sentido frente |
| D5 | `entrada2` | Controle do motor/canal 1, sentido ré |
| D6 | `entrada3` | Controle do motor/canal 2, sentido ré |
| D11 | `entrada4` | Controle do motor/canal 2, sentido frente |

Os pinos D3, D5, D6 e D11 também permitem PWM nas placas Uno/Nano, o que possibilita controlar a velocidade. A correspondência exata entre entradas do driver e motores depende do modelo do driver.

### Sensores e servo

| Arduino | Componente | Pino/sinal |
|---:|---|---|
| D2 | HC-SR04 | `TRIG` traseiro |
| D7 | HC-SR04 | `ECHO` traseiro |
| D9 | Servo | Sinal de controle |
| D4 | LED auxiliar | Reservado no código |
| `SDA`/`SCL` | VL53L0X | Comunicação I2C |

No Arduino Uno, `SDA` é A4 e `SCL` é A5. No Arduino Nano, normalmente também são A4 e A5. Em outras placas, consulte a documentação da placa. Conecte também `VCC` e `GND` dos módulos de acordo com a tensão suportada por eles.

## Montagem e alimentação

1. Fixe os motores, rodas, roda livre e placa no chassi.
2. Instale o servo na frente e fixe o `VL53L0X` sobre o braço do servo.
3. Instale o `HC-SR04` na traseira, apontado para a direção oposta à frente.
4. Conecte os quatro sinais de controle do driver aos pinos indicados.
5. Conecte o servo ao D9 e o `VL53L0X` ao barramento I2C.
6. Conecte `TRIG` ao D2 e `ECHO` ao D7.
7. Conecte o GND do Arduino, do driver e dos sensores em comum.
8. Alimente os motores pela entrada própria do driver, usando uma bateria compatível com os motores.

### Cuidados obrigatórios

- Não alimente os motores pelos pinos do Arduino.
- Não conecte a bateria dos motores diretamente a um pino de sinal.
- Confirme a tensão do `VL53L0X`, do servo e do `HC-SR04` antes de conectar o VCC.
- Não faça alterações na fiação com a bateria conectada.
- Em testes iniciais, mantenha as rodas suspensas ou use um suporte que impeça o carrinho de sair andando.
- Use uma área aberta, sem pessoas, animais, escadas ou objetos frágeis.

## Instalação do software

### 1. Instale o Arduino IDE

Baixe o Arduino IDE no site oficial: [arduino.cc/en/software](https://www.arduino.cc/en/software). Instale a versão adequada ao seu sistema operacional.

### 2. Baixe o projeto

No terminal, execute:

```bash
git clone https://github.com/am4nd6/autonomous-robot-car.git
cd autonomous-robot-car
```

Também é possível baixar o projeto como arquivo ZIP pelo botão **Code > Download ZIP** no GitHub.

### 3. Instale a biblioteca

O sketch usa:

- `Servo.h`, normalmente incluída no Arduino IDE.
- `Adafruit_VL53L0X`, disponível no [repositório oficial da Adafruit](https://github.com/adafruit/Adafruit_VL53L0X).

Para instalar a biblioteca no IDE:

1. Abra **Sketch > Include Library > Manage Libraries**.
2. Pesquise por **Adafruit VL53L0X**.
3. Instale a biblioteca publicada pela Adafruit.
4. Aceite as dependências solicitadas pelo gerenciador.

O projeto **não usa a biblioteca NewPing**, apesar de versões anteriores do README mencionarem esse nome.

## Upload do programa

1. Abra `Code/versao4_carrinho.ino` no Arduino IDE.
2. Conecte o Arduino ao computador pelo cabo USB.
3. Em **Tools > Board**, selecione o modelo correto da sua placa.
4. Em **Tools > Port**, selecione a porta serial correspondente.
5. Clique em **Verify** para compilar.
6. Se a compilação terminar sem erros, clique em **Upload**.
7. Aguarde a mensagem de conclusão.
8. Desconecte o USB somente se a alimentação dos motores estiver instalada de forma segura.

O Arduino IDE pode pedir confirmação para instalar dependências da biblioteca. Isso é esperado. Se a placa não aparecer, instale o driver USB correspondente ao conversor da placa e troque o cabo USB.

## Primeiro teste

Faça os testes nesta ordem:

1. Com as rodas suspensas, ligue somente o Arduino e confirme que o programa inicia.
2. Abra o **Serial Monitor** em `9600 baud`.
3. Observe a mensagem `INICIANDO...`.
4. Verifique se o servo centraliza aproximadamente em 85 graus.
5. Confirme se o sensor VL53L0X inicializa. Se falhar, o LED interno piscará e o programa ficará parado.
6. Com a alimentação dos motores desligada, aproxime objetos dos sensores e observe as mensagens de diagnóstico.
7. Se necessário, descomente `testarmotores();` no `setup()` para testar os motores. Faça isso somente com o carrinho imobilizado.
8. Ligue os motores em baixa velocidade e teste em uma superfície plana e livre.

## Operação e monitor serial

Abra o Monitor Serial em **9600 baud**. O programa pode mostrar mensagens como:

- `INICIANDO...`
- `Failed to boot VL53L0X`
- `Distância da frente`
- `Distância direita`
- `Distância esquerda`
- `Distância de trás`
- `Indo para frente`
- `Indo para trás`
- `Indo para direita`
- `Indo para esquerda`

O LED interno alterna aproximadamente a cada 200 ms enquanto o programa está executando. A mensagem `ping` também é enviada nesse intervalo.

## Configurações ajustáveis

Os valores podem ser alterados no início do sketch:

| Variavel | Valor atual | Efeito |
|---|---:|---|
| `velocidade` | 60.5 | Velocidade nominal de um lado |
| `velocidade2` | 58 | Velocidade nominal do outro lado |
| `velocidade_tras` | 80 | Velocidade de ré |
| `distancia_minima` | 30 cm | Limite para detectar obstáculo |
| `distancia_para_andar` | 50 cm | Espaço considerado suficiente |
| `velocidade_arranque` | 180 | Força inicial dos motores |
| `TEMPO_MOVIMENTO_SERVO` | 200 ms | Tempo de espera após mover o servo |
| `SERVO_CENTRO` | 85 graus | Posição central do sensor |

Ajuste primeiro as velocidades. Valores altos podem fazer o carrinho arrancar bruscamente. Se o carrinho andar torto, ajuste `velocidade` e `velocidade2` gradualmente. Se o servo atingir o fim mecânico, reduza os ângulos de varredura.

## Solução de problemas

### O programa não compila

Confirme se a biblioteca **Adafruit VL53L0X** está instalada e se o arquivo aberto é `versao4_carrinho.ino`. A biblioteca `Servo` deve estar disponível na instalação do Arduino IDE.

### O VL53L0X não inicializa

Desligue a alimentação, revise VCC, GND, SDA e SCL e confirme a tensão do módulo. Verifique também se o sensor não está frouxo sobre o servo. Durante a falha, o código pisca o LED interno e permanece em espera.

### O servo não se move ou vibra

Confira o sinal no D9, a alimentação e o GND comum. Servos podem exigir mais corrente do que a porta USB consegue fornecer; use uma alimentação externa adequada, mantendo o GND compartilhado.

### Os motores não giram

Verifique a alimentação separada do driver, o GND comum, as conexões D3/D5/D6/D11 e o estado de habilitação do driver. Alguns drivers possuem pinos `ENA` e `ENB` ou jumpers que precisam ser configurados.

### O carrinho anda para trás ou vira para o lado errado

Desligue a bateria e inverta os fios do motor afetado ou adapte a lógica das funções de movimento. Nunca altere a fiação com o circuito energizado.

### O carrinho anda torto

Isso pode ocorrer por diferenças entre motores, rodas ou atrito. Ajuste `velocidade` e `velocidade2` em pequenos passos.

### As distâncias parecem erradas

Limpe a janela do VL53L0X, confira o alinhamento dos sensores e evite superfícies muito inclinadas, transparentes ou absorventes. O `HC-SR04` deve estar livre de obstáculos muito próximos e de vibração excessiva.

### O carrinho não para

Desligue imediatamente a alimentação dos motores. Depois verifique a pinagem, a alimentação do driver e a lógica de acionamento. Faça novos testes com as rodas suspensas.

## Limitações e melhorias

Este é um projeto experimental. O código não possui encoders para medir a rotação das rodas, nem controle PID, comunicação remota ou mapa do ambiente. O tempo de movimento é baseado em `delay()`, portanto a precisão das curvas depende da bateria, do piso, dos motores e do peso do chassi.

Possíveis melhorias incluem adicionar encoders, substituir delays por temporização com `millis()`, filtrar leituras inválidas, usar um driver específico documentado, incluir controle de velocidade mais preciso e criar testes automatizados para a lógica de decisão.

## Estrutura do repositório

```text
autonomous-robot-car/
|-- README.md
|-- LICENSE
`-- Code/
    `-- versao4_carrinho.ino
```

## Licença

Este projeto está distribuído sob a [licença MIT](LICENSE). As bibliotecas de terceiros continuam sujeitas às suas próprias licenças.

## Referências

- [Arduino IDE](https://www.arduino.cc/en/software)
- [Adafruit VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X)
- [Documentação do Arduino](https://docs.arduino.cc/)
