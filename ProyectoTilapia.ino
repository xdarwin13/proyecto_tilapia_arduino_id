#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pines para los sensores
#define ONE_WIRE_BUS 4  // Pin para el sensor DS18B20
#define PH_PIN 34       // Pin para el sensor de pH

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

AsyncWebServer server(80);

// Credenciales WiFi
const char* ssid = "Darwin";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  
  // Inicializar los sensores
  ds18b20.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas API para los sensores
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    ds18b20.requestTemperatures();
    float temperature = ds18b20.getTempCByIndex(0);  // Obtener temperatura en Celsius
    String response = String("{\"temperature\":") + temperature + "}";
    AsyncResponseStream *responseStream = request->beginResponseStream("application/json");
    responseStream->print(response);
    responseStream->addHeader("Access-Control-Allow-Origin", "*");
    request->send(responseStream);
  });

  server.on("/ph", HTTP_GET, [](AsyncWebServerRequest *request){
    int sensorValue = analogRead(PH_PIN);
    float voltage = sensorValue * (3.3 / 4095.0);  // Convierte el valor de 0-4095 a voltaje
    float ph = 3.5 * voltage;  // Calcular el valor aproximado de pH (ajusta esta fórmula según el sensor)
    String response = String("{\"ph\":") + ph + "}";
    AsyncResponseStream *responseStream = request->beginResponseStream("application/json");
    responseStream->print(response);
    responseStream->addHeader("Access-Control-Allow-Origin", "*");
    request->send(responseStream);
  });

  server.begin();
}

void loop() {
  // Nada en el loop por ahora
}
