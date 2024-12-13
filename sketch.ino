#include <WiFi.h>
#include <HTTPClient.h>

// Definições dos pinos
#define BUTTON 18       // Botão pressionado
#define LDR 4           // Sensor de luz
#define GREEN_LED 2     // LED verde
#define YELLOW_LED 9    // LED amarelo
#define RED_LED 40      // LED vermelho

// Constantes
const int DEBOUNCE_DELAY = 50;    // Tempo de debouncing em ms
const int LIGHT_THRESHOLD = 600;  // Limite de intensidade para modo noturno
const int GREEN_TIME = 3000;      // 3 segundos
const int YELLOW_TIME = 2000;     // 2 segundos
const int RED_TIME = 5000;        // 5 segundos

// Variáveis globais
unsigned long lastButtonTime = 0;
unsigned long lastYellowLedTime = 0;
unsigned long lastDebounceTime = 0;
int buttonPressCount = 0;
int lastButtonState = HIGH;

void setup() {
  // Inicialização dos pinos
  pinMode(BUTTON, INPUT_PULLUP);   // Botão com resistor pull-up
  pinMode(LDR, INPUT);             // Sensor de luz como entrada
  pinMode(GREEN_LED, OUTPUT);      // LED verde como saída
  pinMode(YELLOW_LED, OUTPUT);     // LED amarelo como saída
  pinMode(RED_LED, OUTPUT);        // LED vermelho como saída

  // Inicializa todos os LEDs apagados
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.begin(9600); // Inicializa a comunicação serial

  // Conexão WiFi
  Serial.print("Conectando ao WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6); // Substitua com suas credenciais WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

void loop() {
  // Leitura do sensor de luz
  int lightReading = digitalRead(LDR);

  // Verifica a intensidade da luz e entra no modo apropriado
  if (lightReading <= LIGHT_THRESHOLD) {
    modoNoturno();  // Modo noturno
  } else {
    modoDiurno();   // Modo diurno
  }
}

void modoNoturno() {
  // Desliga os LEDs verde e vermelho
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Pisca o LED amarelo a cada segundo
  unsigned long currentTime = millis();
  if (currentTime - lastYellowLedTime >= 1000) {
    lastYellowLedTime = currentTime;
    digitalWrite(YELLOW_LED, !digitalRead(YELLOW_LED));
  }
}

void modoDiurno() {
  static int estado = 0;                // Variável para armazenar o estado do semáforo
  static unsigned long tempoAnterior = 0;
  unsigned long currentTime = millis();

  // Verifica o estado do botão com debounce
  verificarBotao();

  switch (estado) {
    case 0: // Verde
      digitalWrite(GREEN_LED, HIGH);    // Acende o LED verde
      digitalWrite(YELLOW_LED, LOW);    // Apaga o LED amarelo
      digitalWrite(RED_LED, LOW);       // Apaga o LED vermelho
      if (currentTime - tempoAnterior >= GREEN_TIME) {
        estado = 1;                      // Muda para o próximo estado (amarelo)
        tempoAnterior = currentTime;
      }
      break;

    case 1: // Amarelo
      digitalWrite(GREEN_LED, LOW);     // Apaga o LED verde
      digitalWrite(YELLOW_LED, HIGH);   // Acende o LED amarelo
      digitalWrite(RED_LED, LOW);       // Apaga o LED vermelho
      if (currentTime - tempoAnterior >= YELLOW_TIME) {
        estado = 2;                      // Muda para o próximo estado (vermelho)
        tempoAnterior = currentTime;
      }
      break;

    case 2: // Vermelho
      digitalWrite(GREEN_LED, LOW);     // Apaga o LED verde
      digitalWrite(YELLOW_LED, LOW);    // Apaga o LED amarelo
      digitalWrite(RED_LED, HIGH);      // Acende o LED vermelho
      if (currentTime - tempoAnterior >= RED_TIME) {
        // Se o botão foi pressionado mais de 3 vezes, envia um alerta
        if (buttonPressCount >= 3) {
          enviarAlerta();                // Envia o alerta
          buttonPressCount = 0;          // Reseta o contador de pressões
        }
        estado = 0;                      // Volta ao estado inicial (verde)
        tempoAnterior = currentTime;
      }
      break;
  }
}

void verificarBotao() {
  static int estadoBotao;
  int leituraBotao = digitalRead(BUTTON);

  // Verifica se houve mudança de estado do botão
  if (leituraBotao != lastButtonState) {
    lastDebounceTime = millis();  // Atualiza o tempo de debouncing
  }

  // Processa a leitura após o tempo de debouncing
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (leituraBotao != estadoBotao) {
      estadoBotao = leituraBotao;
      if (estadoBotao == LOW) {       // Se o botão foi pressionado
        if (digitalRead(RED_LED)) {   // Se o LED vermelho está aceso
          buttonPressCount++;         // Incrementa o contador de pressões
          if (buttonPressCount == 1) {
            delay(1000);              // Espera 1 segundo após a primeira pressão
            modoDiurno();             // Força a mudança para o modo verde
          }
        }
      }
    }
  }

  lastButtonState = leituraBotao;     // Atualiza o estado do botão
}

void enviarAlerta() {
  // Função para enviar um alerta HTTP
  HTTPClient http;
  http.begin("http://www.google.com.br/");  // URL de exemplo para envio de alerta
  int httpResponseCode = http.GET();        // Realiza a requisição GET
  
  if (httpResponseCode > 0) {
    // Se a requisição for bem-sucedida
    Serial.print("Código de resposta HTTP: ");
    Serial.println(httpResponseCode);
  } else {
    // Se ocorrer um erro na requisição
    Serial.print("Código de erro: ");
    Serial.println(httpResponseCode);
  }
  http.end();  // Finaliza a requisição HTTP
}
