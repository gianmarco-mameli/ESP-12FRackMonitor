#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include "private.h"
#include <string.h>
#include <LiquidCrystal_I2C.h>

// wifi
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_broker = "192.168.1.1";
const int mqtt_port = 1883;

const char *client_id = "rackmonitor";

// led declaration
#define RED 14
#define GREEN 12
#define BLUE 13

// dht declaration
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t = 0;
float h = 0;
char tBuffer[15];
char hBuffer[15];

// lcd declaration
LiquidCrystal_I2C lcd(0x27, 16, 2);
char line0[17];
char line1[17];

char *message;

const int interval = 100;
unsigned long previousMillis = 0;

bool messageIncoming = false;
unsigned long messageStartTime = 0;

// define mqtt topics
const char *t_temp = "rackmonitor/temperature";
const char *t_hum = "rackmonitor/humidity";
const char *t_r = "rackmonitor/r";
const char *t_g = "rackmonitor/g";
const char *t_b = "rackmonitor/b";
const char *t_state = "rackmonitor/state";
// const char *t_output = "rackmonitor/output";
const char *t_name = "rackmonitor/name";
const char *t_host = "rackmonitor/host";
// const char *t_message = "rackmonitor/message";
const char *t_status = "rackmonitor/status";

const char *t_bcharge = "rpiusb/bcharge";
const char *t_linev = "rpiusb/linev";
const char *t_timeleft = "rpiusb/timeleft";

const char *motion = "rackmotion/motion";

uint8_t bcharge;
uint8_t timeleft;
int linev;

String ip;

void getDht(void)
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  dtostrf(t, 2, 1, tBuffer);
  client.publish(t_temp, tBuffer);
  itoa(h, hBuffer, 10);
  client.publish(t_hum, hBuffer);
  // Serial.println(message);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char in_message[length];
  unsigned int i = 0;
  for (; i < length; i++)
  {
    in_message[i] = char(payload[i]);
  }
  in_message[i] = '\0';
  messageIncoming = true;
  messageStartTime = millis();
  if (strcmp(topic, t_r) == 0)
  {
    analogWrite(RED, 255 - atoi(in_message));
  }
  if (strcmp(topic, t_g) == 0)
  {
    analogWrite(GREEN, 255 - atoi(in_message));
  }
  if (strcmp(topic, t_b) == 0)
  {
    analogWrite(BLUE, 255 - atoi(in_message));
  }
  if (strcmp(topic, t_bcharge) == 0)
  {
    // sprintf(line1, "%s", in_message);
    bcharge = atoi(in_message);
  }
  if (strcmp(topic, t_timeleft) == 0)
  {
// sprintf(line1, "%16s", in_message);
    timeleft = atoi(in_message);

  }
  if (strcmp(topic, t_linev) == 0)
  {
    // sprintf(line1, "%-16s", in_message);
    linev = atoi(in_message);
  }
  if (strcmp(topic, t_name) == 0)
  {
    sprintf(line0, "%-16s", in_message);
  }
  if (strcmp(topic, t_host) == 0)
  {
    sprintf(line1, "%-16s", in_message);
  }
}

void InitWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print(F("Connecting to WiFi .."));
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect(client_id))
    {
      Serial.println(F("MQTT broker connected"));
      client.publish(t_status, "connected");
      client.subscribe(t_r);
      client.subscribe(t_g);
      client.subscribe(t_b);
      // client.subscribe(t_state);
      // client.subscribe(t_output);
      client.subscribe(t_name);
      client.subscribe(t_host);
      // client.subscribe(t_message);
      client.subscribe(t_bcharge);
      client.subscribe(t_linev);
      client.subscribe(t_timeleft);
    }
    else
    {
      Serial.print(F("failed with state "));
      Serial.print(client.state());
      delay(5000);
      InitWiFi();
    }
  }
}

void InitMqtt()
{
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void setup()
{
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  delay(500);
  InitWiFi();
  delay(500);
  InitMqtt();
  lcd.init();
  lcd.backlight();
  dht.begin();
}

void updateDisplay()
{
  lcd.setCursor(0, 0);
  lcd.print(line0);
  lcd.setCursor(0, 1);
  lcd.print(line1);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    if (!client.connected())
    {
      reconnect();
    }
    getDht();

    client.loop();

    if ((messageIncoming == true && millis() - messageStartTime >= 4000) || messageIncoming == false)
    {
      messageIncoming = false;
      // analogWrite(RED, 0);
      // analogWrite(GREEN, 0);
      // analogWrite(BLUE, 0);
      sprintf(line0, "T: %4s%c T: %3dm", tBuffer, char(223), timeleft);
      sprintf(line1, "C: %3d%%  L: %3dV", bcharge, linev);
    }
    updateDisplay();
  }
}