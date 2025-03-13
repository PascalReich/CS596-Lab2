#include <Arduino.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>
#include "driver/gpio.h"

#define RED_LED_PIN 25
#define YELLOW_LED_PIN 13
#define GREEN_LED_PIN 15

#define BUZZER_PIN 2

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

Adafruit_CAP1188 cap = Adafruit_CAP1188(); // SDA = 21 SCL = 22

int state = 28;
int buzzerState = 0; // 0 off, 1 green, 2 red
int buzzerCounter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.print("GPIO Inititialized");


  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.drawNumber(3, 0, 0, 6);

  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t touched = cap.touched();
  if (touched) {
    state = 0;
    Serial.println("Touched!");
  }

  if (state < 20) {
    // green
    digitalWrite(RED_LED_PIN, 0);
    digitalWrite(YELLOW_LED_PIN, 0);
    digitalWrite(GREEN_LED_PIN, 1);

    buzzerState = 1;
    goto end;
  }

  if (state < 28) {
    // yellow
    digitalWrite(RED_LED_PIN, 0);
    digitalWrite(YELLOW_LED_PIN, 1);
    digitalWrite(GREEN_LED_PIN, 0);

    buzzerState = 0;
    buzzerCounter = 0;
    goto end;
  }

  if (state < 68) {
    // red
    digitalWrite(RED_LED_PIN, 1);
    digitalWrite(YELLOW_LED_PIN, 0);
    digitalWrite(GREEN_LED_PIN, 0);

    buzzerState = 2;
    goto end;
  }

  if (state < 76) {
    // red-yellow
    digitalWrite(RED_LED_PIN, 1);
    digitalWrite(YELLOW_LED_PIN, 1);
    digitalWrite(GREEN_LED_PIN, 0);

    buzzerState = 0;
    buzzerCounter = 0;
  } else {
    buzzerState = 1;
    digitalWrite(RED_LED_PIN, 0);
    digitalWrite(YELLOW_LED_PIN, 0);
    digitalWrite(GREEN_LED_PIN, 1);
  }



  // idle in green and wait for a touch

  end: 
  //tft.drawNumber(state, 0, 0, 6);
  Serial.println(state);

  if (buzzerState == 1) {
    if (buzzerCounter < 2) tone(BUZZER_PIN, 50);
    else noTone(BUZZER_PIN);
    buzzerCounter += 1;
    if (buzzerCounter >= 8) buzzerCounter = 0;
  } else if (buzzerState == 2) {
     if (buzzerCounter) tone(BUZZER_PIN, 50);
     else noTone(BUZZER_PIN);
     buzzerCounter = !buzzerCounter;
  } else {
    noTone(BUZZER_PIN);
  }

  // turn buzzer off and on. 
  // digitalWrite(???, 1)

  if (state < 100) state++;
  
  delay(250);

}


/*

250ms as min period

Red 40 periods
(1 off and 1 on buzzer)
Red-yellow - 8 periods
green n-periods
green after touch 20 periods
(6 periods off and 2 on buzzer)
yellow 8 periods


*/
