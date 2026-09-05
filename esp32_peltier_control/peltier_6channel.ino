/*
 * ESP32 펠티어 6채널 제어 + 요청 시 즉시 상태 응답
 *
 * 핀: GPIO 32, 33, 25, 26, 27, 14
 * PWM 주파수: 20kHz
 *
 * 제어 입력: "채널번호,퍼센트\n" (예: 3,30 -> 3번 채널 30%)
 * 상태 요청: "S" 입력 시, 현재 6채널 레벨(0~3)을 6자리로 즉시 응답
 *
 * 중요: baud rate 9600 (Python 데이터수집 스크립트와 반드시 일치해야 함)
 *
 * 회로:
 *   파워서플라이(+) 12V -> 퓨즈(1차, 20A) -> MOSFET Vin(+)
 *   ESP32 GPIO(PWM) -> MOSFET 게이트
 *   MOSFET Vout(+) -> 퓨즈(2차, 20A) -> 펠티어(+)
 *   MOSFET Vout(-) -> 펠티어(-) 직결
 *   파워서플라이(-), MOSFET GND, ESP32 GND -> 공통 GND 라인
 */

const int pwmPins[6] = {32, 33, 25, 26, 27, 14};
const int freq = 20000;
const int resolution = 8;

int currentDuty[6] = {0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) {
    ledcAttach(pwmPins[i], freq, resolution);
    ledcWrite(pwmPins[i], 0);
  }
}

int dutyToLevel(int duty) {
  int percent = duty * 100 / 255;
  if (percent == 0) return 0;
  else if (percent <= 33) return 1;
  else if (percent <= 66) return 2;
  else return 3;
}

void reportStatus() {
  for (int i = 0; i < 6; i++) {
    Serial.print(dutyToLevel(currentDuty[i]));
  }
  Serial.println();
}

void loop() {
  if (Serial.available()) {
    char firstChar = Serial.peek();

    if (firstChar == 'S') {
      Serial.read();
      reportStatus();
    } else {
      int channel = Serial.parseInt();
      Serial.read();
      int percent = Serial.parseInt();
      if (Serial.read() == '\n') {
        channel = constrain(channel, 1, 6) - 1;
        percent = constrain(percent, 0, 100);
        int duty = percent * 255 / 100;
        ledcWrite(pwmPins[channel], duty);
        currentDuty[channel] = duty;
      }
    }
  }
}
