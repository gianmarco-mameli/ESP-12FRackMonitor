#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include "secrets.h"
// #include <iostream>   // std::cout
// #include <string>     // std::string, std::stoi

// led declaration
const int RED = 14;
const int GREEN = 12;
const int BLUE = 15;

// dht declaration
#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t = 0;
float h = 0;

// pir declaration
#define PIRPIN 13
int pir = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

// define mqtt topics
const char *temp = "rackmonitor/temperature";
const char *hum = "rackmonitor/humidity";
const char *motion = "rackmonitor/motion";
const char *r = "rackmonitor/r";
const char *g = "rackmonitor/g";
const char *b = "rackmonitor/b";

WiFiClient espClient;
String ip;

PubSubClient client(espClient);
const char *client_id = "rackmonitor";


void write_temp(float temp, char C_F)
{
  // display.drawRect(1, 1, display.width() - 1, display.height() - 1, WHITE);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(104, 3);
  display.print("o");
  display.setTextSize(2);
  display.setCursor(112, 10);
  display.print(C_F);
  display.setTextSize(3);
  display.setCursor(10, 6);
  display.print(temp);
  display.display();
}


int getPir(void)
{
  int pir = digitalRead(PIRPIN);
  char *message = "";
  itoa(pir,message,10);
  client.publish(motion,message);
  // Serial.println(message);
  return pir;
}

void getDht(void) {
  // delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t) )
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  char *message = "";
  dtostrf(t,4,2,message);
  client.publish(temp,message);
  itoa(h,message,10);
  client.publish(hum,message);
  // Serial.println(message);
}

void showInfo(void)
{
  // display.drawRect(1, 1, display.width() - 1, display.height() - 1, WHITE);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(102, 5);
  display.print("o");
  display.setTextSize(2);
  display.setCursor(110, 12);
  display.print("C");
  display.setTextSize(3);
  display.setCursor(8, 8);
  display.print(t);

  display.setTextSize(1);
  display.setCursor(8, 50);
  display.print(WiFi.localIP());

  display.display();
}


void connectWifi(void)
{
  // display.drawRect(1, 1, display.width() - 1, display.height() - 1, WHITE);
  display.setTextSize(2);
  display.setCursor(15, 10);
  display.print("Connecting to Wifi");
  // display.display();
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED)
  {
    // Serial.print("Connected. IP: ");
    // Serial.println(WiFi.localIP());
    display.setCursor(15, 20);
    display.print("Connected. IP: ");
    display.println(WiFi.localIP());
    // display.display();
  }
  else
  {
    Serial.println("Connection Failed!");
  }
  // Serial.println("Connected to the WiFi network");
  display.setCursor(15, 30);
  display.println("Connected to the WiFi");
  // display.display();
}
void callback(char *topic, byte *payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg = msg + (char) payload[i];  // convert *byte to string
  }
  // Serial.println(topic);
  // Serial.println(msg);
  if (strcmp(topic,r)==0) {
    // if (msg == "1") {
    //   digitalWrite(RED,HIGH);
    //   Serial.println("Red on");
    // } else {
      // int value = stoi(msg);
      analogWrite(RED,msg.toInt());
    // }
  }
  if (strcmp(topic,g)==0) {
    // if (msg == "0") {
    //   digitalWrite(GREEN,HIGH);
    //   Serial.println("Green on");
    // } else {
      analogWrite(GREEN,msg.toInt());
    // }
  }
  if (strcmp(topic,b)==0) {
    // if (msg == "0") {
    //   digitalWrite(BLUE,HIGH);
    //   Serial.println("Blue on");
    // } else {
      analogWrite(BLUE,msg.toInt());
    // }
  }
}

void InitMqtt() {
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    Serial.println("Connecting to MQTT broker.....");
    if (client.connect(client_id)) {
      Serial.println("MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
}

void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  // display.setRotation(2);
  display.display();
  delay(2000);
  display.clearDisplay();
  connectWifi();
  InitMqtt();
  pinMode(PIRPIN, INPUT);
  dht.begin();
  // display.fillRect(1, 1, display.width() - 1, display.height() - 1, WHITE);
  // delay(2000);

  client.subscribe(r);
  client.subscribe(g);
  client.subscribe(b);
}

void loop()
{
  client.loop();
  getDht();
  int pir = getPir();
  delay(1000);
  if (pir == 1)
  {
    showInfo();
    display.clearDisplay();
  }
  else if (pir == 0)
  {
    display.display();
    display.clearDisplay();
  }
}
