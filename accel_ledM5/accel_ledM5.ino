//時間が経つにつれ色が変わっていくLEDを、加速度によって変わる時間を短くする。ボタンCでON/OFF
#include <Adafruit_NeoPixel.h>
#include <M5Stack.h>
#include <Wire.h>
#include "utility/MPU9250.h"
#include "utility/quaternionFilters.h"

#define PIN 5       //信号端子のピン番号
#define LED_NUM 15  //LEDの数
#define THR 100   //変化する際の加速度の基準1
#define THR2 50   //変化する際の加速度の基準2

void M5set(int x);  //x行目をクリアしカーソルを合わせる

Adafruit_NeoPixel ledtape = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);  //3つめのパラメータは使うLEDテープの制御ICがWS2812ならNEO_KHZ800 / WS2811ならNEO_KHZ400
MPU9250 IMU;

uint16_t r=1;
int flag = 0;

void setup() {
  M5.begin();
  ledtape.begin();
  ledtape.clear();
  ledtape.show();   //一旦全てOFFの状態で反映
  Wire.begin(21,22);
  pinMode(PIN, OUTPUT);
  M5.Lcd.printf("LIGHTING UP DOG LEAD");
  IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
}

void loop() {
 uint16_t i, j;
 if (M5.BtnC.wasPressed()) {
  M5set(10);
  M5.Lcd.printf("LIGHT UP");
  flag=1;
 }
 
 if(flag==1){
  IMU.readGyroData(IMU.gyroCount);
  IMU.getGres();
  IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes; 
  IMU.updateTime();
  if(IMU.gx>THR || IMU.gx<-THR){ r=5.0;}
  else if(IMU.gx>THR2 || IMU.gx<-THR2){ r=3.0;}
  else{ r=1.0;}
  
  for(j=0; j <256; j+=r) {
   for(i=0; i < ledtape.numPixels(); i++) {
    ledtape.setPixelColor(i, rotateColor((((i) * 256 / ledtape.numPixels()) + j) & 255));
   }
   ledtape.show();
   M5.update();
   if(M5.BtnC.wasPressed()){
    M5set(10);
    flag=0;
    break;
   }
   delay(20);
  }
 }
 else{
  ledtape.clear();
  ledtape.show();
  delay(1);
 }
 M5.update();
}
 
//RGBの色の移り変わり関数
uint32_t rotateColor(byte WheelPos) {
  if(WheelPos < 85) {
   return ledtape.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ledtape.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ledtape.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void M5set(int x){
  M5.Lcd.setCursor(0, x);
  M5.Lcd.fillRect(0, x, 60, x, BLACK);
}
