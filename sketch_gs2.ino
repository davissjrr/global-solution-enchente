#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <time.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST"; // wifi
const char* password = "";        // senha

const char* mqtt_broker = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_client_id = "esp32-sensor-";
const char* mqtt_topic_publish = "sensor/agua/davis1";
const char* mqtt_topic_subscribe = "buzzer/control/davis1";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// componentes
LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd 16x2

#define TRIG_PIN     5
#define ECHO_PIN     18
#define DHT_PIN      4
#define BUZZER_PIN   19

#define DHT_TYPE     DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// limites
const float LIMITE_ENCHENTE_CM = 10.0;
const float LIMITE_UMIDADE = 80.0;

// funções
void conectarWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void configurarHora() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) Serial.println("Erro ao obter hora");
}

String obterHorario() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "00:00:00";
  char buffer[9];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return String(buffer);
}

float medirNivelAgua() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  return duracao * 0.034 / 2;
}

void exibirDados(float nivel, float umid, float temp, bool enchente, bool chuva) {
  lcd.setCursor(0, 0);
  lcd.print("Niv:");
  lcd.print(nivel, 0);
  lcd.print(" U:");
  lcd.print(umid, 0);

  lcd.setCursor(0, 1);
  if (enchente) {
    lcd.print("ENCHENTE ");
    lcd.print(obterHorario());
  } else if (chuva) {
    lcd.print("CHUVA ");
    lcd.print(obterHorario());
  } else {
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.print(" ºC");
  }
}

// callback mqtt
void callback(char* topic, byte* payload, unsigned int length) {
  if (payload[0] == '1') {
    tone(BUZZER_PIN, 2000);
  } else if (payload[0] == '0') {
    noTone(BUZZER_PIN);
  }
}


// setup
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  dht.begin();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  conectarWiFi();
  configurarHora();

  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(callback);
  mqttClient.connect(mqtt_client_id);
  mqttClient.subscribe(mqtt_topic_subscribe);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema I]iniciado");
  delay(2000);
  lcd.clear();
}

// loop
void loop() {
  mqttClient.loop();

  float nivelAgua = medirNivelAgua();
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    lcd.setCursor(0, 0);
    lcd.print("Erro DHT22    ");
    delay(2000);
    return;
  }


  bool enchente = (nivelAgua >= LIMITE_ENCHENTE_CM);
  bool chuva = (umidade >= LIMITE_UMIDADE);

  exibirDados(nivelAgua, umidade, temperatura, enchente, chuva);

  // publicação via MQTT
  String horario = obterHorario();
  String mensagem = "Horario:" + horario +
                    " Nivel:" + String(nivelAgua) +
                    " Umid:" + String(umidade) +
                    " Temp:" + String(temperatura);
  mqttClient.publish(mqtt_topic_publish, mensagem.c_str());


  // alarme local
  if (enchente || chuva) {
    tone(BUZZER_PIN, 2000);
  } else {
    noTone(BUZZER_PIN);
  }
}
