#include "DHT.h"


// DHT11 센서 설정
#define DHTPIN 2        // DHT 센서 핀
#define DHTTYPE DHT11   // DHT11 센서 타입


// 스텝 모터 신호핀 설정
const int motorPin1 = 8;    // IN1
const int motorPin2 = 9;    // IN2
const int motorPin3 = 10;   // IN3
const int motorPin4 = 11;   // IN4


// 스텝 모터의 스텝 설정
const int steps[] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001, B0000};


// 모터 제어 관련 변수
const int stepsPerRevolution = 2048;  // 한 바퀴 회전에 필요한 스텝 수 (모터 사양에 따라 다를 수 있음)
const int stepsFor180 = stepsPerRevolution / 2;  // 180도 회전에 필요한 스텝 수
int currentPosition = 0;  // 현재 모터 위치
int motorSpeed = 1000;    // 스텝 사이의 지연시간(마이크로초)
bool isAt180 = false;     // 180도 위치 여부를 추적


DHT dht(DHTPIN, DHTTYPE);  // DHT 객체 초기화


void setup() {
  // 모터 신호핀을 출력으로 설정
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("DHT11 and Stepper Motor Control Test");
  dht.begin();
}


void loop() {
  // 습도 읽기
  float h = dht.readHumidity();
  
  // 센서 데이터가 유효하지 않으면 다시 읽기
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // 습도 출력
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  
  // 습도가 60% 이상일 때 180도로 회전
  if (h > 60 && !isAt180) {
    Serial.println("Motor: Rotating to 180 degrees");
    rotateTo180();
    isAt180 = true;
  } 
  // 습도가 60% 미만일 때 0도로 복귀
  else if (h <= 60 && isAt180) {
    Serial.println("Motor: Returning to 0 degrees");
    returnToZero();
    isAt180 = false;
  }
  
  delay(2000); // 2초마다 센서 값을 읽음
}


// 180도 위치로 회전
void rotateTo180() {
  for(int i = 0; i < stepsFor180; i++) {
    clockwise();
  }
  currentPosition = stepsFor180;
}


// 0도 위치로 복귀
void returnToZero() {
  for(int i = 0; i < stepsFor180; i++) {
    counterClockwise();
  }
  currentPosition = 0;
}


// 한 스텝 시계 방향 회전
void clockwise() {
  for(int i = 7; i >= 0; i--) {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}


// 한 스텝 반시계 방향 회전
void counterClockwise() {
  for(int i = 0; i < 8; i++) {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}


// 모터 정지
void motorStop() {
  motorSignalOutput(8); // 8번째 신호(B0000)를 출력하여 정지
}


// 모터 신호 출력 함수
void motorSignalOutput(int out) {
  digitalWrite(motorPin1, bitRead(steps[out], 0));
  digitalWrite(motorPin2, bitRead(steps[out], 1));
  digitalWrite(motorPin3, bitRead(steps[out], 2));
  digitalWrite(motorPin4, bitRead(steps[out], 3));
}
