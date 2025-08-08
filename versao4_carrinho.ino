#include <Servo.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X sensor_da_frente = Adafruit_VL53L0X();

// Declaração de constantes relacionadas às direções do sensor:
#define TRAS 0
#define FRENTE 1
#define DIREITA 2
#define DIREITA2 3
#define ESQUERDA 4
#define ESQUERDA2 5
#define SERVO_CENTRO 85
#define TEMPO_MOVIMENTO_SERVO 200


// Declaração de variáveis:
int entrada1 = 3;
int entrada2 = 5;
int entrada3 = 6;
int entrada4 = 11;
float velocidade = 60.5;  //Roda esquerda.
float velocidade2 = 58;   //Roda direita (mais rápida).
int velocidade_tras = 80;
int distancia_minima = 30;
int distancia_para_andar = 50;
bool parado;
bool parado2 = true;
int ultimo_movimento = 6;
int velocidade_arranque = 180;
int led = 4;

// Declaração dos pinos:
#define pino_trigger_tras 2
#define pino_echo_tras 7

// Declaração das variáveis de distância relacionadas às direções do sensor:
float distancia_esquerda;
float distancia_esquerda2;
float distancia_direita;
float distancia_direita2;
float distancia_frente;
float distancia_tras;

Servo myservo;  // Cria um objeto baseado na biblioteca Servo.

// Função para fazer o robô andar para frente:
void ir_parafrente() {
  if (parado2 == true) {
    analogWrite(entrada1, velocidade_arranque);
    digitalWrite(entrada2, LOW);
    digitalWrite(entrada3, LOW);
    analogWrite(entrada4, velocidade_arranque);
    delay(100);
    parado2 = false;
  }

  analogWrite(entrada1, velocidade);
  digitalWrite(entrada2, LOW);
  digitalWrite(entrada3, LOW);
  analogWrite(entrada4, velocidade2);
  parado = false;
}

// Função para fazer o robô parar:
void parar() {
  digitalWrite(entrada1, LOW);
  digitalWrite(entrada2, LOW);
  digitalWrite(entrada3, LOW);
  digitalWrite(entrada4, LOW);
  parado = true;
  parado2 = true;
}

// Função para fazer o robô andar para trás:
void ir_paratras() {
  if (parado2 == true) {
    digitalWrite(entrada1, LOW);
    analogWrite(entrada2, velocidade_arranque);
    analogWrite(entrada3, velocidade_arranque);
    digitalWrite(entrada4, LOW);
    delay(100);
    parado2 = false;
  }

  digitalWrite(entrada1, LOW);
  analogWrite(entrada2, velocidade_tras);
  analogWrite(entrada3, velocidade_tras);
  digitalWrite(entrada4, LOW);
}

// Função para fazer o robô andar para a esquerda:
void ir_paraesquerda() {
  if (parado2 == true) {
    digitalWrite(entrada1, LOW);
    analogWrite(entrada2, velocidade_arranque);  // esquerda para trás
    digitalWrite(entrada3, LOW);  // direita parada
    digitalWrite(entrada4, LOW);
    delay(100);
    parado2 = false;
  }

  digitalWrite(entrada1, LOW);
  analogWrite(entrada2, velocidade);  // roda esquerda para trás
  digitalWrite(entrada3, LOW);         // roda direita parada
  digitalWrite(entrada4, LOW);
}

// Função para fazer o robô andar para a direita:
void ir_paradireita() {
  if (parado2 == true) {
    digitalWrite(entrada1, LOW);  // esquerda parada
    digitalWrite(entrada2, LOW);

    analogWrite(entrada3, velocidade_arranque);  // direita para trás
    digitalWrite(entrada4, LOW);
    delay(100);
    parado2 = false;
  }

  digitalWrite(entrada1, LOW);  // roda esquerda parada
  digitalWrite(entrada2, LOW);
  analogWrite(entrada3, velocidade);  // roda direita para trás
  digitalWrite(entrada4, LOW);
}

// Função para testar motores:
void testarmotores() {
  ir_parafrente();
  delay(1000);
  ir_paradireita();
  delay(1000);
  ir_paraesquerda();
  delay(1000);
  parar();
  delay(1000);
  ir_paratras();
  delay(1000);
  parar();
}

// Função que lê a distância com base na direção ordenada:
float ler_distancia(int lado) {
  static float distancia_atual;

  if (lado == TRAS) {
    // Envia um pulso de 10 microsegundos para acionar o sensor ultrassônico:
    digitalWrite(pino_trigger_tras, HIGH);
    delayMicroseconds(10);
    digitalWrite(pino_trigger_tras, LOW);

    // Calcula a distância com base no tempo de retorno do pulso de ultrassom:
    unsigned int tempo;
    tempo = pulseIn(pino_echo_tras, HIGH);
    distancia_atual = (tempo * 0.034) / 2.0;

  } else {
    // Usa o sensor VL53L0X com o servo para direções diferentes
    if (lado == DIREITA) {
      myservo.write(10);
    } else if (lado == DIREITA2) {
      myservo.write(45);
    } else if (lado == ESQUERDA2) {
      myservo.write(115);
    } else if (lado == ESQUERDA) {
      myservo.write(150);
    } else {
      myservo.write(85);  // FRENTE
    }
    delay(TEMPO_MOVIMENTO_SERVO);  // Espera o servo se posicionar

    sensor_da_frente.readRange();

    while (!sensor_da_frente.isRangeComplete());

    uint16_t distancia_mm = sensor_da_frente.readRange();

    if (distancia_mm != 8191) {
      if (sensor_da_frente.timeoutOccurred()) {
        Serial.println("Falha no sensor VL53L0X. Usando valor alto de distância.");
        distancia_atual = 999;  // Valor alto para indicar falha
      } else {
        distancia_atual = distancia_mm / 10.0;  // mm -> cm
      }
    }
  }

  if (distancia_atual < 0) {  // Evitar leituras negativas.
    distancia_atual = 0;
  }

  return distancia_atual;
}

// Função que armazena as distâncias nas diferentes direções enquanto o robô está parado:
void ler_distancias_parado() {
  distancia_frente = ler_distancia(FRENTE);
  distancia_direita = ler_distancia(DIREITA);
  myservo.write(85);
  delay(TEMPO_MOVIMENTO_SERVO);
  distancia_esquerda = ler_distancia(ESQUERDA);
  myservo.write(85);
  delay(TEMPO_MOVIMENTO_SERVO);
  distancia_tras = ler_distancia(TRAS);
}

// Função que calcula a maior distância obtida enquanto o robô está parado:
int calcular_maior_distancia() {
  int maior_distancia;
  float maior_valor;

  ler_distancias_parado();
  maior_valor = distancia_frente;
  maior_distancia = FRENTE;

  Serial.println("\tDistância da frente: " + String(maior_valor));

  //////////////////////////////
  // Situação em que não há espaço livre à frente:
  if (distancia_frente < distancia_minima) {
    Serial.println("Não tem espaço na frente!");

    // Situação em que não há espaço livre à frente, mas há espaço disponível à direita ou à esquerda:
    if ((distancia_direita >= distancia_para_andar) || (distancia_esquerda >= distancia_para_andar)) {
      Serial.println("Há espaço livre para os lados");

      // Verificação de qual dos dois lados, direita ou esquerda, apresenta a maior distância:
      if (distancia_direita > maior_valor) {
        maior_valor = distancia_direita;
        maior_distancia = DIREITA;
      }

      if (distancia_esquerda > maior_valor) {
        maior_valor = distancia_esquerda;
        maior_distancia = ESQUERDA;
      }

      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));

    } else if ((distancia_tras >= distancia_para_andar) && (distancia_direita < distancia_para_andar) && (distancia_esquerda < distancia_para_andar)) {  // Situação em que o carrinho está bloqueado à frente, não está bloqueado nas laterais, mas o maior espaço livre é atrás.
      Serial.println("\tDistância de trás: " + String(distancia_tras));
      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      maior_valor = distancia_tras;
      maior_distancia = TRAS;

    } else if ((distancia_tras >= distancia_para_andar) && (distancia_direita < distancia_minima) && (distancia_esquerda < distancia_minima)) {  // Situação em que o carrinho está bloqueado à frente, está bloqueado nas laterais, mas tem espaço livre atrás.
      Serial.println("\tDistância de trás: " + String(distancia_tras));
      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      maior_valor = distancia_tras;
      maior_distancia = TRAS;

    } else if ((distancia_tras < distancia_minima) && (distancia_direita < distancia_minima) && (distancia_esquerda < distancia_minima)) {  // Situação em que o carrinho está completamente encurralado e, por isso, tentará sair movendo-se na direção com o maior espaço disponível entre as quatro opções.
      Serial.println("\tCarrinho tá encurralado: ");
      // Verificação de qual dos três lados (direita, esquerda ou atrás) possui a maior distância.
      if (distancia_direita > maior_valor) {
        maior_valor = distancia_direita;
        maior_distancia = DIREITA;
      }

      if (distancia_esquerda > maior_valor) {
        maior_valor = distancia_esquerda;
        maior_distancia = ESQUERDA;
      }

      if (distancia_tras > maior_valor) {
        maior_valor = distancia_tras;
        maior_distancia = TRAS;
      }

      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      Serial.println("\tDistância de trás: " + String(distancia_tras));
    }
  }
  //////////////////////////////
  if (ultimo_movimento == TRAS) {
    // Situação em que o último movimento do carrinho foi para trás:

    //Há espaço disponível à direita ou à esquerda:
    if ((distancia_direita >= distancia_para_andar) || (distancia_esquerda >= distancia_para_andar)) {
      Serial.println("Há espaço livre para os lados");

      // Verificação de qual dos dois lados, direita ou esquerda, apresenta a maior distância:
      if (distancia_direita > maior_valor) {
        maior_valor = distancia_direita;
        maior_distancia = DIREITA;
      }

      if (distancia_esquerda > maior_valor) {
        maior_valor = distancia_esquerda;
        maior_distancia = ESQUERDA;
      }

      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));

    } else if ((distancia_tras >= distancia_para_andar) && (distancia_direita < distancia_minima) && (distancia_esquerda < distancia_minima)) {  // Situação em que o carrinho está bloqueado nas laterais, mas tem espaço livre atrás.
      Serial.println("\tDistância de trás: " + String(distancia_tras));
      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      maior_valor = distancia_tras;
      maior_distancia = TRAS;

    } else if ((distancia_tras >= distancia_para_andar) && (distancia_direita < distancia_para_andar) && (distancia_esquerda < distancia_para_andar)) {  // Situação em que o carrinho não está bloqueado nas laterais, mas o maior espaço livre é atrás.
      Serial.println("\tDistância de trás: " + String(distancia_tras));
      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      maior_valor = distancia_tras;
      maior_distancia = TRAS;

    } else if ((distancia_tras < distancia_minima) && (distancia_direita < distancia_minima) && (distancia_esquerda < distancia_minima)) {  // Situação em que o carrinho está completamente encurralado e, por isso, tentará sair movendo-se na direção com o maior espaço disponível entre as quatro opções.
      Serial.println("\tCarrinho tá encurralado: ");
      // Verificação de qual dos três lados (direita, esquerda ou atrás) possui a maior distância.
      if (distancia_direita > maior_valor) {
        maior_valor = distancia_direita;
        maior_distancia = DIREITA;
      }

      if (distancia_esquerda > maior_valor) {
        maior_valor = distancia_esquerda;
        maior_distancia = ESQUERDA;
      }

      if (distancia_tras > maior_valor) {
        maior_valor = distancia_tras;
        maior_distancia = TRAS;
      }

      Serial.println("\tDistância direita: " + String(distancia_direita));
      Serial.println("\tDistância esquerda: " + String(distancia_esquerda));
      Serial.println("\tDistância de trás: " + String(distancia_tras));
    }
  }
  ultimo_movimento = maior_distancia;
  return maior_distancia;
}

// Função que recebe uma direção na qual o robô irá entrar em movimento:
void andar(int valor) {
  if (valor == FRENTE) {
    Serial.println("\nIndo para frente");
    ir_parafrente();
  } else if (valor == TRAS) {
    Serial.println("\nIndo para trás");
    ir_paratras();
    delay(300);
    parar();
  } else if (valor == ESQUERDA) {
    Serial.println("\nIndo para esquerda");
    parado2 = true;
    ir_paraesquerda();  //Vai para a esquerda na velocidade de arranque.
    ir_paraesquerda();
    delay(300);
    parar();
  } else {
    Serial.println("\nIndo para direita");
    parado2 = true;
    ir_paradireita();  //Vai para a direita na velocidade de arranque.
    ir_paradireita();
    delay(300);
    parar();
  }
}

//Função setup:
void setup() {
  myservo.attach(9);  // Diz que o objeto "myservo" está ligado ao pino 9.
  myservo.write(SERVO_CENTRO);
  Serial.begin(9600);
  Serial.println("INICIANDO...");
  pinMode(pino_trigger_tras, OUTPUT);
  pinMode(pino_echo_tras, INPUT);
  pinMode(entrada1, OUTPUT);
  pinMode(entrada2, OUTPUT);
  pinMode(entrada3, OUTPUT);
  pinMode(entrada4, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //testarmotores();

  Serial.println("Adafruit VL53L0X test.");
  if (!sensor_da_frente.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
  // power
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));

  // start continuous ranging
  sensor_da_frente.startRangeContinuous();
}

// Função loop:
void loop() {
  //ir_parafrente();
  //return;
  blink();

  /*if (sensor_da_frente.isRangeComplete()) {
    Serial.print("Distance in mm: ");
    Serial.println(sensor_da_frente.readRange());
  }
  return;*/
  int distancia;
  distancia = calcular_maior_distancia();  // Armazenando o retorno da função que calcula a maior distância.
  // Função que faz o robô se mover com base no maior lado:
  andar(distancia);

  // Enquanto não estiver parado (no caso o carrinho estará se movendo para frente):
  while (!parado) {
    // Se encontrar um obstáculo em alguma das três direções enquanto se move, o robô irá parar:
    blink();
    if ((ler_distancia(FRENTE) < distancia_minima) || (ler_distancia(DIREITA2) < distancia_minima) || (ler_distancia(FRENTE) < distancia_minima) || (ler_distancia(ESQUERDA2) < distancia_minima)) {
      parar();  // Função que faz o carrinho parar.
    }
  }
}

void blink() {
  static unsigned int lastTime = millis();
  if ((millis() - lastTime) >= 200) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    lastTime = millis();
    Serial.println("ping");
  }
}