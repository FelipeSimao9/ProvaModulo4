# Manual do Semáforo Inteligente IoT

Aluno: Felipe Freire Machado Simão

## Modos de Operação do Sistema
### 1.1 Inicialização
Ao iniciar, o sistema começa com todos os LEDs apagados e realiza a conexão com a rede Wi-Fi. Esse é o estado inicial antes do funcionamento normal do semáforo.

### 1.2 Modo Noturno
Este modo é acionado quando o sensor de luminosidade (LDR) detecta uma baixa intensidade de luz (leitura ≤ 600):

O LED amarelo começa a piscar a cada 1 segundo
Os LEDs verde e vermelho permanecem desligados
O sistema permanece neste estado até que a luminosidade ambiente aumente
### 1.3 Modo Diurno (Operação Regular)
Esse modo é ativado quando há luz suficiente (leitura > 600) e o sistema passa a operar em ciclos, com três fases:

#### 1.3.1 Fase do Sinal Verde (3 segundos)
O LED verde está aceso
Os outros LEDs estão apagados
Permite a passagem livre
#### 1.3.2 Fase do Sinal Amarelo (2 segundos)
O LED amarelo é aceso
Os outros LEDs estão apagados
Indica que o veículo deve se preparar para parar
#### 1.3.3 Fase do Sinal Vermelho (5 segundos)
O LED vermelho está aceso
Os outros LEDs estão apagados
Indica a necessidade de parar
### 1.4 Situações Especiais

#### 1.4.1 Botão para atravessar 
Quando o semáforo está no sinal vermelho e o botão de travessia é pressionado:

O sistema aguarda 1 segundo
O semáforo muda para verde para permitir a travessia
Após a travessia, o sistema retorna ao funcionamento normal
#### 1.4.2 Aviso de emergencia
Se o botão for pressionado 3 vezes enquanto o semáforo estiver no sinal vermelho:

O sistema envia uma requisição HTTP para um servidor
O código de resposta da requisição será exibido no monitor serial
Após o envio do alerta, o sistema continua a operação normalmente
## 2. Regras de Transição
### 2.1 Troca de Modos
De Normal para Noturno: Ocorre quando o LDR detecta uma leitura ≤ 600.
De Noturno para Normal: Ocorre quando a leitura do LDR se torna > 600.
### 2.2 Mudança de Estados (Modo Normal)
Verde → Amarelo: Após 3 segundos de sinal verde.
Amarelo → Vermelho: Após 2 segundos de sinal amarelo.
Vermelho → Verde: Após 5 segundos de sinal vermelho, ou imediatamente após a pressão do botão de travessia.
## 3. Gestão de Eventos
### 3.1 Sistema de Debounce do Botão
A implementação do debounce utiliza um tempo de espera de 50 ms.
Esse processo evita leituras errôneas causadas por interferência elétrica.
Garante que a contagem de pressionamentos seja precisa e sem erros.
### 3.2 Contagem de Pressionamentos
O contador de pressionamentos é ativado apenas quando o semáforo está no sinal vermelho.
A contagem é reiniciada sempre que um alerta é enviado.
São necessários 3 pressionamentos para que o alerta de emergência seja acionado.
## 4. Detalhes Técnicos
### 4.1 Conexão Wi-Fi
A conexão com a rede Wi-Fi é essencial para enviar alertas para um servidor.
O sistema aguarda até que a conexão seja estabelecida antes de iniciar a operação.
O status da conexão é exibido no monitor serial para acompanhamento.
### 4.2 Sensor de Luminosidade
O sistema realiza a leitura digital do sensor LDR.
O valor de intensidade de luz tem um limite configurável (atualmente 600).
Com base na leitura do LDR, o modo de operação do semáforo é determinado.