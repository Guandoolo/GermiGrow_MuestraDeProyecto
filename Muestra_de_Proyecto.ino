#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT11 Sensor
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;

// Humedad en tierra Sensores
const int humsuelo1 = 34;  // Primer sensor de humedad en tierra
const int humsuelo2 = 35; 
//const int humsuelo2 = 14;  // Segundo sensor de humedad en tierra
int valHumsuelo1;
int valHumsuelo2;

// LDR Sensor
const int LDR = 14;
int ValorLDR;
float luzPorcentaje;

// Relay
const int pinRele = 15;
const int maxHumedadSuelo = 30;  // Umbral de humedad del suelo

// WiFi Credenciales
//const char *ssid = "IUB.STUDENTS";
//const char *password = "1U3.res.soc";

const char *ssid = "Familia Cera";
const char *password = "D1001892527a";

//const char *ssid = "TECNO";
//const char *password = "prueba2.0";

const char *apiUrl = "http://127.0.0.1:8000"; // URL de la API

/***************Caracteres Especiales*************/
byte humeS[] = {
  B00000, B00000, B00000, B11111, B11111, B11111, B11111, B01110
};

byte tempChar[] = {
  B00100, B01010, B01010, B01010, B01110, B11111, B11111, B01110
};

byte humeChar[] = {
  B00100, B00100, B01110, B01110, B11111, B11111, B11111, B01110
};

byte errorChar[] = {
  B00000, B10101, B01010, B10101, B00000, B01010, B11111, B00000
};

byte solChar[] = {
  B00000, B00000, B10000, B00100, B11000, B11101, B11110, B11110
};

byte wifiChar[] = {
  B00000, B00000, B00000, B00001, B00011, B00111, B01111, B11111
};

/************************************************/

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  // Create special characters
  lcd.createChar(0, humeS);
  lcd.createChar(1, tempChar);
  lcd.createChar(2, humeChar);
  lcd.createChar(3, errorChar);
  lcd.createChar(4, solChar);
  lcd.createChar(5, wifiChar);
  lcd.home();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  const int maxAttempts = 20;

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(250);
    Serial.print("Conectando");
    Serial.println(".");
    
    lcd.setCursor(0, 0);
    lcd.print("Conectando");
    lcd.setCursor(0, 1);
    lcd.print(".");
    
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Conexion establecida");
    Serial.println("Dirección IP: ");
    
    lcd.setCursor(0, 0);
    lcd.print("Conexion WIFI");
    lcd.write(byte(5));
    lcd.setCursor(0, 1);
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("No se pudo conectar a la red WiFi");
    
    lcd.setCursor(0, 0);
    lcd.print("No se pudo");
    lcd.setCursor(0, 1);
    lcd.print("Conectar a WiFi");
    delay(5000);
  }

  // Initialize DHT11 Sensor
  Serial.println(F("Prueba del sensor DHTxx"));
  dht.begin();

  // Initialize pins
  pinMode(humsuelo1, INPUT);
  pinMode(humsuelo2, INPUT);
  pinMode(LDR, INPUT);
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("BIENVENIDOS");
  lcd.setCursor(2, 1);
  lcd.print("I feel good");

  delay(1000);
}

void loop() {
  //humedadSuelo();
  sensores();
  //sendDato();

  // Control del relé basado en la humedad del suelo (solo usando el primer sensor)
    if (valHumsuelo1 < maxHumedadSuelo) {
    digitalWrite(pinRele, HIGH);  // Activa el relé
    Serial.println("Relé activado: Riego encendido");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Rele Activado");
    lcd.setCursor(0, 1);
    lcd.print(" Regando");
  } else {
    digitalWrite(pinRele, LOW);  // Desactiva el relé
    Serial.println("Relé desactivado: Riego apagado");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Rele Desactivado");
    lcd.setCursor(0, 1);
    lcd.print(" Riego Apagado");
  }

  delay(1000);

}

/*           Json para mandar los datos           */
/*
void sendDato(){
  
  // Ejecutar la función solo si no se ha ejecutado

    HTTPClient http;  // Declarar una instancia de HTTPClient
    String url = String(apiUrl) + "/create_lectura";

    http.begin(url); // Asignar URL
    http.addHeader("Content-Type", "application/json");

    // Obtener la hora del servidor NTP
    timeClient.update();
    String hora_colombiana = timeClient.getFormattedTime();

    // Crear un JSON con los valores necesarios
    const size_t bufferSize = JSON_OBJECT_SIZE(7);
    DynamicJsonDocument doc(bufferSize);

    // Asignar los valores del JSON
    doc["id_info"] = id;
    doc["temp_ambiente"] = String(temperature); // Convertir el float a String para el JSON
    doc["hume_ambiente"] = String(humidity);
    doc["hume_suelo"] = valHumsuelo1;
    doc["luz_porcen"] = luzPorcentaje; // Valor de suciedad en porcentaje
    doc["fecha"] = fecha_actual;
    doc["hora"] = hora_colombiana;  
    doc["id_planta"] = id_planta;

    String output;
    serializeJson(doc, output);
    Serial.println(output);

    int httpResponseCode = http.POST(output);  // Realizar la solicitud POST con el JSON

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Respuesta POST con datos JSON: " + payload);
    } else {
      Serial.println("Error en la solicitud POST con datos JSON");
    }

}
*/

void sensores() {
  /**********************Sensor DTH11********************/
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Error al leer la humedad!"));
    Serial.println(F("Error al leer la temperatura!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(3));
    lcd.print(" Error con");
    lcd.setCursor(0, 1);
    lcd.print(" Los sensores");
  } else {
    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(2));
    lcd.print(humidity);
    lcd.print("% ");
    lcd.write(byte(1));
    lcd.print(temperature);
    lcd.print("C ");
  }

  /**********************Sensores Humedad En Suelo********************/

  valHumsuelo1 = analogRead(humsuelo1);
  valHumsuelo1 = map(valHumsuelo1, 4095, 1700, 0, 99);
  //durante la calibracion 1700 fue lo mas bajo que saco, lo que quiere decir que seria lo mas mojado que detecta el sensor
  valHumsuelo2 = analogRead(humsuelo2);
  valHumsuelo2 = map(valHumsuelo2, 4095, 1700, 0, 99);

  // Print values
  Serial.print("Humedad del suelo 1: ");
  Serial.print(valHumsuelo1);
  Serial.println(" %");

  Serial.print("Humedad del suelo 2: ");
  Serial.print(valHumsuelo2);
  Serial.println(" %");

  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.print(valHumsuelo1);
  lcd.print("%-1 ");
  lcd.write(byte(0));
  lcd.print(valHumsuelo2);
  lcd.print("%-2 ");

  /**********************Sensor LDR********************/
  ValorLDR = analogRead(LDR);
  int VMinimo = 0; // Es cuando m
  int VMaximo = 3000; // Es cuando mas luz hay

  luzPorcentaje = map(ValorLDR, VMinimo, VMaximo, 0, 100);

  Serial.print("Porcentaje de luz: ");
  Serial.print(luzPorcentaje);
  Serial.println(" %");

  lcd.setCursor(12, 1);
  lcd.write(byte(4));
  lcd.print(ValorLDR);

  delay(2000);
  Serial.println("----");
  Serial.println("");

  
}
/*
void humedadSuelo() {
  valHumsuelo1 = analogRead(humsuelo1);
  valHumsuelo1 = map(valHumsuelo1, 4095, 1700, 0, 99);
  //durante la calibracion 1700 fue lo mas bajo que saco, lo que quiere decir que seria lo mas mojado que detecta el sensor
  valHumsuelo2 = analogRead(humsuelo2);
  valHumsuelo2 = map(valHumsuelo2, 4095, 1700, 0, 99);

  // Print values
  Serial.print("Humedad del suelo 1: ");
  Serial.print(valHumsuelo1);
  Serial.println(" %");

  Serial.print("Humedad del suelo 2: ");
  Serial.print(valHumsuelo2);
  Serial.println(" %");

  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.print(valHumsuelo1);
  lcd.print("% ");
  lcd.write(byte(0));
  lcd.print(valHumsuelo2);
  lcd.print("% ");
}
*/