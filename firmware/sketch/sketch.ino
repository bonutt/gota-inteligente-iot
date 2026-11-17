// Gota Inteligente - kit IoT de irrigação inteligente (ESP32)
// FIAP Global Solution 2026.2 - Physical Computing (IoT & IoB)

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

// Pinos
const int PINO_SOLO        = 34;  // sensor de umidade do solo (potenciômetro no Wokwi)
const int PINO_CHUVA       = 35;  // chance de chuva no modo simulado
const int PINO_MODO        = 14;  // chave: ligada = previsão real | desligada = simulada
const int PINO_DHT         = 15;
const int PINO_RELE        = 26;  // bomba d'água
const int PINO_LED_VERDE   = 25;
const int PINO_LED_AMARELO = 33;
const int PINO_LED_VERM    = 32;
const int PINO_BOTAO       = 4;   // irrigação manual

// Wi-Fi e local do sítio (Ibiúna-SP)
const char* WIFI_SSID  = "Wokwi-GUEST";
const char* WIFI_SENHA = "";
const float LATITUDE   = -23.656;
const float LONGITUDE  = -47.222;
const char* CODIGO_DISPOSITIVO = "gota-001";

// Regras de decisão (em %)
const float UMIDADE_MINIMA  = 35.0;  // abaixo disso o solo está seco
const float UMIDADE_MAXIMA  = 70.0;  // acima disso não precisa irrigar
const float UMIDADE_CRITICA = 15.0;  // abaixo disso irriga mesmo se for chover
const float CHUVA_ALTA      = 70.0;  // chance de chuva para segurar a bomba

const unsigned long INTERVALO_LEITURA  = 2000;    // 2 s
const unsigned long INTERVALO_PREVISAO = 600000;  // 10 min
const unsigned long DURACAO_MANUAL     = 10000;   // 10 s

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;

enum Acao { IRRIGAR, NAO_IRRIGAR, AGUARDAR_CHUVA };

float previsaoOnline = -1;  // -1 = ainda sem previsão
unsigned long ultimaPrevisao = 0;
unsigned long ultimaLeitura = 0;
unsigned long manualAte = 0;
bool botaoAntes = HIGH;
bool primeiraPrevisao = true;

const char* nomeAcao(Acao a) {
  if (a == IRRIGAR) return "IRRIGAR";
  if (a == AGUARDAR_CHUVA) return "AGUARDAR_CHUVA";
  return "NAO_IRRIGAR";
}

float lerPercentual(int pino) {
  return analogRead(pino) * 100.0 / 4095.0;
}

// Índice de 0 a 100: 70% falta de água no solo + 20% calor + 10% ar seco
int calcularIndiceEstresse(float solo, float temp, float umidAr) {
  float deficit = constrain((UMIDADE_MAXIMA - solo) / UMIDADE_MAXIMA, 0.0, 1.0);
  float calor   = constrain((temp - 25.0) / 15.0, 0.0, 1.0);
  float arSeco  = constrain((60.0 - umidAr) / 60.0, 0.0, 1.0);
  return (int)round(deficit * 70 + calor * 20 + arSeco * 10);
}

const char* nivelEstresse(int indice) {
  if (indice >= 70) return "CRITICO";
  if (indice >= 40) return "ATENCAO";
  return "NORMAL";
}

// Cruza a umidade do solo com a previsão de chuva
Acao decidir(float solo, float temp, float umidAr, float chuva, String& motivo) {
  bool chuvaAlta = chuva >= CHUVA_ALTA;
  if (solo >= UMIDADE_MAXIMA) {
    motivo = "Solo umido, nao precisa irrigar.";
    return NAO_IRRIGAR;
  }
  if (solo < UMIDADE_MINIMA) {
    if (chuvaAlta && solo >= UMIDADE_CRITICA) {
      motivo = "Solo seco, mas vai chover. Bomba desligada para economizar agua e energia.";
      return AGUARDAR_CHUVA;
    }
    if (chuvaAlta) {
      motivo = "Solo em nivel critico: irrigar mesmo com chuva prevista.";
      return IRRIGAR;
    }
    motivo = "Solo seco e pouca chance de chuva. Ligar a bomba.";
    return IRRIGAR;
  }
  if (chuvaAlta) {
    motivo = "Solo na faixa ideal e chuva prevista. Nao irrigar.";
    return NAO_IRRIGAR;
  }
  if (solo < UMIDADE_MINIMA + 10 && temp >= 32 && umidAr < 40) {
    motivo = "Calor forte e ar seco com solo perto do minimo: irrigacao preventiva.";
    return IRRIGAR;
  }
  motivo = "Solo na faixa ideal. Nenhuma acao necessaria.";
  return NAO_IRRIGAR;
}

void conectarWiFi() {
  Serial.print("[WiFi] Conectando a ");
  Serial.print(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA, 6);
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 10000) {
    delay(250);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(" conectado! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" sem conexao. Seguindo no modo offline.");
  }
}

// Maior chance de chuva das próximas 3 horas (API Open-Meteo)
float buscarPrevisaoChuva() {
  if (WiFi.status() != WL_CONNECTED) return -1;
  WiFiClientSecure cliente;
  cliente.setInsecure();
  HTTPClient http;
  String url = "https://api.open-meteo.com/v1/forecast?latitude=" + String(LATITUDE, 3) +
               "&longitude=" + String(LONGITUDE, 3) +
               "&hourly=precipitation_probability&forecast_hours=3&timezone=America%2FSao_Paulo";
  http.setTimeout(8000);
  if (!http.begin(cliente, url)) return -1;
  float maior = -1;
  if (http.GET() == 200) {
    String corpo = http.getString();
    int ini = corpo.indexOf("\"precipitation_probability\":[");
    if (ini >= 0) {
      ini = corpo.indexOf('[', ini) + 1;
      String valores = corpo.substring(ini, corpo.indexOf(']', ini));
      int pos = 0;
      while (pos < (int)valores.length()) {
        int virgula = valores.indexOf(',', pos);
        if (virgula < 0) virgula = valores.length();
        float v = valores.substring(pos, virgula).toFloat();
        if (v > maior) maior = v;
        pos = virgula + 1;
      }
    }
  }
  http.end();
  return maior;
}

void mostrarNoLCD(float solo, float chuva, bool manual, Acao acao, int indice) {
  char linha1[17];
  snprintf(linha1, sizeof(linha1), "Solo%3d%% Chv%3d%%", (int)round(solo), (int)round(chuva));
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  if (manual)                      lcd.print("IRRIGACAO MANUAL");
  else if (acao == IRRIGAR)        lcd.print("BOMBA LIGADA    ");
  else if (acao == AGUARDAR_CHUVA) lcd.print("AGUARDANDO CHUVA");
  else if (indice >= 70)           lcd.print("ESTRESSE CRITICO");
  else                             lcd.print("SOLO OK         ");
}

void setup() {
  Serial.begin(115200);
  pinMode(PINO_MODO, INPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AMARELO, OUTPUT);
  pinMode(PINO_LED_VERM, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  dht.setup(PINO_DHT, DHTesp::DHT22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Gota Inteligente");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...    ");

  Serial.println();
  Serial.println("==============================================");
  Serial.println(" GOTA INTELIGENTE - kit IoT de irrigacao");
  Serial.print(" Dispositivo: ");
  Serial.println(CODIGO_DISPOSITIVO);
  Serial.println("==============================================");
  conectarWiFi();
}

void loop() {
  unsigned long agora = millis();

  // Botão manual: 10 s de irrigação
  bool botao = digitalRead(PINO_BOTAO);
  if (botaoAntes == HIGH && botao == LOW) {
    manualAte = agora + DURACAO_MANUAL;
    Serial.println(">> Irrigacao MANUAL acionada pelo agricultor (10 s).");
  }
  botaoAntes = botao;

  if (agora - ultimaLeitura < INTERVALO_LEITURA) {
    delay(20);
    return;
  }
  ultimaLeitura = agora;

  // Sensores
  float solo = lerPercentual(PINO_SOLO);
  TempAndHumidity ar = dht.getTempAndHumidity();
  float temp = isnan(ar.temperature) ? 25.0 : ar.temperature;
  float umidAr = isnan(ar.humidity) ? 60.0 : ar.humidity;

  // Previsão de chuva: real (internet) ou simulada (potenciômetro)
  float chuva;
  const char* fonte;
  if (digitalRead(PINO_MODO) == HIGH) {
    if (primeiraPrevisao || agora - ultimaPrevisao > INTERVALO_PREVISAO) {
      float p = buscarPrevisaoChuva();
      if (p >= 0) previsaoOnline = p;
      ultimaPrevisao = agora;
      primeiraPrevisao = false;
    }
    if (previsaoOnline >= 0) {
      chuva = previsaoOnline;
      fonte = "Open-Meteo";
    } else {
      chuva = lerPercentual(PINO_CHUVA);
      fonte = "simulada (sem internet)";
    }
  } else {
    chuva = lerPercentual(PINO_CHUVA);
    fonte = "simulada";
  }

  // Decisão
  String motivo;
  Acao acao = decidir(solo, temp, umidAr, chuva, motivo);
  int indice = calcularIndiceEstresse(solo, temp, umidAr);
  bool manual = agora < manualAte;
  bool bomba = manual || acao == IRRIGAR;

  // Bomba, LEDs e LCD
  digitalWrite(PINO_RELE, bomba ? HIGH : LOW);
  digitalWrite(PINO_LED_VERDE, (!bomba && acao == NAO_IRRIGAR && indice < 70) ? HIGH : LOW);
  digitalWrite(PINO_LED_AMARELO, (acao == AGUARDAR_CHUVA && !manual) ? HIGH : LOW);
  digitalWrite(PINO_LED_VERM, indice >= 70 ? HIGH : LOW);
  mostrarNoLCD(solo, chuva, manual, acao, indice);

  // Monitor serial: decisão, motivo e telemetria (JSON) da leitura
  char json[220];
  snprintf(json, sizeof(json),
           "{\"codigo_dispositivo\":\"%s\",\"umidade_solo\":%.1f,\"temperatura_ar\":%.1f,"
           "\"umidade_ar\":%.1f,\"probabilidade_chuva\":%.0f,\"bomba_ligada\":%s}",
           CODIGO_DISPOSITIVO, solo, temp, umidAr, chuva, bomba ? "true" : "false");
  Serial.printf("Solo %3.0f%% | Ar %.1fC %2.0f%% | Chuva %3.0f%% (%s) | Estresse %d %s | %s -> bomba %s%s\n",
                solo, temp, umidAr, chuva, fonte, indice, nivelEstresse(indice), nomeAcao(acao),
                bomba ? "LIGADA" : "DESLIGADA", manual ? " (manual)" : "");
  Serial.print("  Motivo: ");
  Serial.println(motivo);
  Serial.print("  Telemetria: ");
  Serial.println(json);
}
