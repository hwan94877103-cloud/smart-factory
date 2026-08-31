/*
 * DS18B20 센서 1개씩 주소 확인용 코드
 *
 * 사용법:
 *   1. 센서 1개만 연결 (풀업저항 4.7kΩ, 디지털 2번 핀)
 *   2. 업로드 후 시리얼 모니터(9600)에서 주소 확인 및 메모
 *   3. 센서를 바꿔 끼우고 리셋 버튼 눌러서 다음 센서 주소 확인
 *   4. 이 과정을 필요한 센서 개수만큼 반복
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(" ");
  }
}

void setup() {
  Serial.begin(9600);
  sensors.begin();

  int count = sensors.getDeviceCount();
  Serial.print("발견된 센서 개수: ");
  Serial.println(count);

  if (count > 0) {
    DeviceAddress addr;
    sensors.getAddress(addr, 0);
    Serial.print("센서 주소: ");
    printAddress(addr);
    Serial.println();
  } else {
    Serial.println("센서를 찾을 수 없습니다. 배선을 확인하세요.");
  }
}

void loop() {
  // 한 번만 확인하면 되므로 비워둠
}
