#define BLYNK_TEMPLATE_ID "TMPL60lvmkPau"
#define BLYNK_TEMPLATE_NAME "TEMP HUMIDITY"
#define LEDC_CHANNEL 0

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <RTClib.h>

char auth[] = "rw53z20fiO9vRsRFrkbEJ9ZvVuNwtWl4";
char ssid[] = "Mobitel_4G_B6801";
char pass[] = "Tharindu130";

#define DHTTYPE DHT11
#define DHTPIN 4
#define SOIL_MOISTURE_PIN 35
#define RELAY_PIN 5
#define RELAY_PIN2 4
#define BUZZER_PIN 18
#define LED_PIN 15
#define PUSH_BUTTON_PIN 13
#define NEW_PUSH_BUTTON_PIN 26  // New push button
#define SDA 21
#define SCL 22
#define ldrPin1 34
#define ldrPin2 32

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire);
RTC_DS3231 rtc;

bool ledState = false;
bool relayState = false;
bool relayState2 = false;
bool buzzerActivated = false;
bool showDateTimeFrame = true;
BlynkTimer timer;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;  // Increased debounce delay for the first button
unsigned long lastButton2DebounceTime = 0;
unsigned long button2DebounceDelay = 200;  // Increased debounce delay for the second button
unsigned long lastFrameChange = 0;
const unsigned long frameChangeInterval = 5000;

void controlBuzzer(bool state) {
  if (state && !buzzerActivated) {
    ledcWrite(LEDC_CHANNEL, 255);
    delay(10);
    ledcWrite(LEDC_CHANNEL, 0);
    buzzerActivated = true;
  } else if (!state) {
    ledcWrite(LEDC_CHANNEL, 0);
    buzzerActivated = false;
  }
}

void updateRelayState(bool newState) {
  digitalWrite(RELAY_PIN, newState);
  relayState = newState;
  Blynk.virtualWrite(V1, relayState);
  controlBuzzer(relayState);
}

void controlLed(bool state) {
  digitalWrite(LED_PIN, state);
  ledState = state;
  Blynk.virtualWrite(V0, ledState);
  controlBuzzer(ledState);
}

void sendSensor() {
  Serial.println("Reading sensors...");

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Send notification if soil moisture level is less than 20%
  

  int sensorValue1 = analogRead(ldrPin1);
  int sensorValue2 = analogRead(ldrPin2);

  // Calculate average
  int averageValue = (sensorValue1 + sensorValue2) / 2;

  Serial.print("Sensor 1: ");
  Serial.println(sensorValue1);
  Serial.print("Sensor 2: ");
  Serial.println(sensorValue2);

  int moisturePercentage = map(soilMoisture, 1750, 4095, 100, 0);
  moisturePercentage = constrain(moisturePercentage, 0, 100);

  if (moisturePercentage < 20) {
    Blynk.logEvent("soil_moisture","Hey Plant enthusiast, Your Baby Getting Dry...");
  }

  if (moisturePercentage <= 75 && !relayState) {
    updateRelayState(true);
  } else if (moisturePercentage >= 100 && relayState) {
    updateRelayState(false);
  }
  if (averageValue <= 2000 && !ledState) {
    controlLed(true);
  } else if (averageValue > 2000 && ledState) {
    controlLed(false);
  }
  if (h < 75) {
    // Turn on relay
    digitalWrite(RELAY_PIN2, HIGH);
    Serial.println("Relay ON");
    relayState2 = true;
  } else {
    // Turn off relay
    digitalWrite(RELAY_PIN2, LOW);
    Serial.println("Relay OFF");
    relayState2 = false;
  }
  Serial.print("Relay 2 Status : ");
  Serial.println(relayState2);

  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 0);
  display.println("BotaniTech Pot");

  if (showDateTimeFrame) {
    display.setTextSize(1);
    display.setCursor(22, 18);
    DateTime now = rtc.now();
    display.println("Date: " + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));
    display.println("    Time: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
  } else {
    display.setTextSize(1);
    display.setCursor(0, 8);
    display.println("Temperature: " + String(t) + " C");
    display.println("Humidity: " + String(h) + " %");
    display.println("Soil Moisture: " + String(moisturePercentage) + " %");
    display.println("Growing Lights: " + String(ledState ? "ON" : "OFF"));
    display.println("Motor: " + String(relayState ? "ON" : "OFF"));
  }

  display.display();

  unsigned long currentTime = millis();
  if (currentTime - lastFrameChange >= frameChangeInterval) {
    showDateTimeFrame = !showDateTimeFrame; // Switch frames
    lastFrameChange = currentTime; // Reset timer
  }

  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V7, moisturePercentage);

  controlBuzzer(relayState);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);


  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  pinMode(NEW_PUSH_BUTTON_PIN, INPUT_PULLUP);  // New push button

  ledcSetup(LEDC_CHANNEL, 5000, 10);
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);

  timer.setInterval(1000L, sendSensor);

  Blynk.begin(auth, ssid, pass);
  rtc.begin();

  while (!Serial) {
    delay(10); // Wait for serial monitor to open
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  Blynk.run();
  timer.run();

  unsigned long currentMillis = millis();

  int buttonState = digitalRead(PUSH_BUTTON_PIN);
  if (buttonState == LOW && currentMillis - lastDebounceTime > debounceDelay) {
    lastDebounceTime = currentMillis;
    if (ledState) {
      controlLed(false);
    } else {
      controlLed(true);
    }
  }

  int button2State = digitalRead(NEW_PUSH_BUTTON_PIN);  // New push button
  if (button2State == LOW && currentMillis - lastButton2DebounceTime > button2DebounceDelay) {
    lastButton2DebounceTime = currentMillis;
    if (relayState) {
      updateRelayState(false);
    } else {
      updateRelayState(true);
    }
  }
}
