# Monitor de Chuvas e Enchentes com ESP32

Este projeto utiliza um ESP32 para monitorar o nível de água, a umidade e a temperatura ambiente, alertando sobre possíveis enchentes e chuvas intensas. Os dados são exibidos em um display LCD 16x2 e também enviados via MQTT para monitoramento remoto. Um buzzer é acionado localmente para alertas.

---

## Componentes Utilizados

- **ESP32**
- Sensor ultrassônico HC-SR04 (para medir nível de água)
- Sensor de temperatura e umidade DHT22
- Display LCD 16x2 com interface I2C
- Buzzer
- Conexão Wi-Fi para envio de dados via MQTT

---

## Funcionalidades

- Medição do nível de água em centímetros usando sensor ultrassônico.
- Leitura da umidade relativa do ar e temperatura com sensor DHT22.
- Exibição dos dados no LCD, indicando quando há enchente ou chuva.
- Envio dos dados via protocolo MQTT para um broker público.
- Controle remoto do buzzer via mensagens MQTT.
- Acionamento automático do buzzer local em caso de enchente ou chuva intensa.
- Registro do horário das medições usando NTP para timestamps.

---

## Instalação e Uso

1. Configure as credenciais Wi-Fi no código (`ssid` e `password`).
2. Ajuste os pinos conforme o seu circuito.
3. Configure os limites de enchente (`LIMITE_ENCHENTE_CM`) e umidade (`LIMITE_UMIDADE`) conforme necessário.
4. Faça o upload do código para o ESP32 usando a IDE Arduino ou outra de sua preferência.
5. Inscreva-se no tópico MQTT para receber comandos do buzzer e visualize as mensagens no tópico de publicação.
6. Monitore o display LCD para leituras locais e alertas.

---

## Formato da Mensagem MQTT Publicada

As mensagens enviadas para o tópico `sensor/agua/` seguem o formato:

```
Horario:<HH:MM:SS> Nivel:<valor_cm> Umid:<valor_%> Temp:<valor_C>
```

Exemplo:

```
Horario:14:41:33 Nivel:13.45 Umid:78.00 Temp:25.60
```

---

## Link do Wokwi

https://wokwi.com/projects/432865038271897601

## Autores

- Henrique Celso  
- Davis Junior  
- Jhonatan Silva  
