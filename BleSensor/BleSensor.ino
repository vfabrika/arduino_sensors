#include <DHT.h>
#include <DHT_U.h>

#include <SoftwareSerial.h>
#include <FastLED.h>

SoftwareSerial btSerial(3, 2);
String command;

long potMillis = 0;
long dhtMillis = 0;

#define POT_SEND_INTERVAL 500
#define DHT_SEND_INTERVAL 2000

#define DHTPIN 7
#define DHTTYPE DHT11

#define POT_PIN A0

DHT dht(DHTPIN, DHTTYPE);

bool isOpen = false;

#define LED_PIN_1     4
#define LED_PIN_2     10

#define NUM_LEDS    8
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

int movingLedIndex = -1;

void setup()
{
  pinMode(POT_PIN, INPUT);
  delay(500);

  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  delay(200);
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  delay(200);
  Serial.begin(9600);
  delay(200);

  btSerial.begin(9600);
  dht.begin();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (btSerial.available()) {

    char readChar = btSerial.read();

    if (readChar != '#')
      command += readChar;
    else
      doCommand();

    Serial.write(readChar);
  }

  if (Serial.available()) {
    btSerial.write(Serial.read());
  }

  checkInterval(currentMillis);
}

void checkInterval(long currentMillis)
{
  if (currentMillis - dhtMillis >= DHT_SEND_INTERVAL)
  {
    dhtMillis = currentMillis;

    float tempIn = dht.readTemperature();
    String serialData = String("temp=") + tempIn + String("#pressure=") + random(0, 100) / 10.0 + String("#");
    btSerial.println(serialData);
    Serial.println(serialData);
  }
  if  (currentMillis - potMillis >= POT_SEND_INTERVAL)
  {
    potMillis = currentMillis;

    String analogData = String("pot=") + analogRead(POT_PIN) + String("#");
    btSerial.println(analogData);
  }
}


void doCommand()
{
  if (command == "toggle")
  {
    toggleLeds();
  }
  else if (command == "on")
  {
    setAllColor(CRGB::Purple);
    isOpen = true;
  }
  else if (command == "off")
  {
    setAllColor(CRGB::Black);
    isOpen = false;
  }
  else if (command == "up")
  {
    movingLedIndex++;
    if (movingLedIndex > 15)
      movingLedIndex = 15;

    turnOnSpecificLed(CRGB::Purple);
  }
  else if (command == "down")
  {
    movingLedIndex--;
    if (movingLedIndex < 0)
      movingLedIndex = 0;

    turnOnSpecificLed(CRGB::Purple);
  }
  else if (command.indexOf("=") > 0)
  {
    String variableName = command.substring(0, command.indexOf("="));
    String variableValueString = command.substring(command.indexOf("=") + 1);

    Serial.println("variableName: " + variableName);
    Serial.println("variableValueString: " + variableValueString);

    if (variableName == "brightness")
    {
      int variableValue = variableValueString.toInt();

      FastLED.setBrightness(variableValue);
      FastLED.show();
    }
  }

  command = "";
}

void toggleLeds()
{
  if (isOpen)
  {
    setAllColor(CRGB::Black);
    isOpen = false;
  }
  else
  {
    setAllColor(CRGB::Purple);
    isOpen = true;
  }
}

void setAllColor(CRGB color)
{
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    leds1[i] = color;
    leds2[i] = color;
  }

  FastLED.show();
}

void turnOnSpecificLed(CRGB color)
{
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    leds1[i] = CRGB::Black;
    leds2[i] = CRGB::Black;
  }

  int tempIndex = movingLedIndex;

  if (tempIndex > 7)
  {
    tempIndex = 15 - tempIndex;
    leds2[tempIndex] = color;
  }
  else
    leds1[tempIndex] = color;

  FastLED.show();

}

