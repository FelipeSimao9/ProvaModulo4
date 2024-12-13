#include <WiFi.h>
#include <HTTPClient.h>

// Definições dos pinos
#define GREEN_LED 2     // LED verde
#define RED_LED 40      // LED vermelho
#define YELLOW_LED 9    // LED amarelo
#define BUTTON 18       // Botão pressionado
#define LDR 4           // Sensor de luz

// Constantes
const int LIGHT_THRESHOLD = 600;  // Limite de intensidade para modo noturno
const int DEBOUNCE_DELAY = 50;    // Tempo de debouncing em ms
const int GREEN_TIME = 3000;      // 3 segundos
const int YELLOW_TIME = 2000;     // 2 segundos
const int RED_TIME = 5000;        // 5 segundos

// Variáveis globais
unsigned long lastDebounceTime = 0;
unsigned long lastYellowLedTime = 0;
int lastButtonState = HIGH;
int buttonPressCount = 0;
unsigned long lastButtonTime = 0;

void setup() {
  // Configuração dos pinos
  pinMode(GREEN_LED, OUTPUT);    // LED verde como saída
  pinMode(RED_LED, OUTPUT);      // LED vermelho como saída
  pinMode(YELLOW_LED, OUTPUT);   // LED amarelo como saída
  pinMode(BUTTON, INPUT_PULLUP); // Botão com resistor pull-up
  pinMode(LDR, INPUT);           // Sensor de luz como entrada

  // Inicializa todos os LEDs apagados
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  Serial.begin(9600);  // Inicializa a comunicação serial

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

  if (lightReading <= LIGHT_THRESHOLD) {
    // Se a intensidade de luz for baixa, entra no modo noturno
    nightMode();
  } else {
    // Caso contrário, entra no modo diurno
    dayMode();
  }
}

void nightMode() {
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

void dayMode() {
  static int state = 0;                // Variável para armazenar o estado do semáforo
  static unsigned long previousTime = 0;
  unsigned long currentTime = millis();

  // Verifica o estado do botão com debounce
  checkButton();

  switch (state) {
    case 0: // Verde
      digitalWrite(GREEN_LED, HIGH);    // Acende o LED verde
      digitalWrite(YELLOW_LED, LOW);    // Apaga o LED amarelo
      digitalWrite(RED_LED, LOW);       // Apaga o LED vermelho
      if (currentTime - previousTime >= GREEN_TIME) {
        state = 1;                      // Muda para o próximo estado (amarelo)
        previousTime = currentTime;
      }
      break;

    case 1: // Amarelo
      digitalWrite(GREEN_LED, LOW);     // Apaga o LED verde
      digitalWrite(YELLOW_LED, HIGH);   // Acende o LED amarelo
      digitalWrite(RED_LED, LOW);       // Apaga o LED vermelho
      if (currentTime - previousTime >= YELLOW_TIME) {
        state = 2;                      // Muda para o próximo estado (vermelho)
        previousTime = currentTime;
      }
      break;

    case 2: // Vermelho
      digitalWrite(GREEN_LED, LOW);     // Apaga o LED verde
      digitalWrite(YELLOW_LED, LOW);    // Apaga o LED amarelo
      digitalWrite(RED_LED, HIGH);      // Acende o LED vermelho
      if (currentTime - previousTime >= RED_TIME) {
        // Se o botão foi pressionado mais de 3 vezes, envia um alerta
        if (buttonPressCount >= 3) {
          sendAlert();                 // Chama a função para enviar o alerta
          buttonPressCount = 0;        // Reseta o contador de pressões
        }
        state = 0;                      // Volta ao estado inicial (verde)
        previousTime = currentTime;
      }
      break;
  }
}

void checkButton() {
  static int buttonState;
  int reading = digitalRead(BUTTON);

  // Verifica mudanças de estado do botão
  if (reading != lastButtonState) {
    lastDebounceTime = millis();     // Atualiza o tempo de debouncing
  }

  // Se passou o tempo de debouncing, processa a leitura
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {       // Se o botão foi pressionado
        if (digitalRead(RED_LED)) {   // Se o LED vermelho está aceso
          buttonPressCount++;         // Incrementa o contador de pressões
          if (buttonPressCount == 1) {
            delay(1000);              // Espera 1 segundo após a primeira pressão
            dayMode();                // Força a mudança para o modo verde
          }
        }
      }
    }
  }

  lastButtonState = reading;         // Atualiza o estado do botão
}

void sendAlert() {
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
