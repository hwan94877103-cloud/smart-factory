/*
 * L298N 모터드라이버로 수중펌프 속도 제어
 *
 * 배선:
 *   D9  -> ENA (PWM 속도 제어)
 *   D8  -> IN1 (방향)
 *   D7  -> IN2 (방향)
 *   L298N GND -> 아두이노 GND -> 외부전원(12V 어댑터) GND (공통)
 *   외부전원 12V -> L298N 전원 입력
 *   L298N OUT1/OUT2 -> 수중펌프
 *
 *   참고: 펌프 정격 6~12V(권장 6~9V), L298N 전압강하(약 2V) 감안 시
 *         펌프에는 약 10V 전달됨 (정격 범위 안, 안전)
 *
 * 사용법: 시리얼 모니터에 0~255 사이 숫자 입력 후 엔터
 */

const uint8_t ENA = 9;
const uint8_t IN1 = 8;
const uint8_t IN2 = 7;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  Serial.begin(9600);
  Serial.println("속도 입력하세요 (0~255):");
}

void loop() {
  if (Serial.available()) {
    int speed = Serial.parseInt();
    if (Serial.read() == '\n') {
      speed = constrain(speed, 0, 255);
      analogWrite(ENA, speed);
      Serial.print("속도 설정: ");
      Serial.println(speed);
    }
  }
}
