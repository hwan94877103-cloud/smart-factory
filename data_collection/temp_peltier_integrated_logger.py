"""
온도센서(아두이노, 7채널) + 펠티어 제어값(ESP32)을 하나의 CSV로 통합 저장

온도 데이터 수신을 트리거로 삼아, 그 즉시 ESP32에 현재 6채널 상태를 요청하여
정확히 같은 시점의 온도-제어값 데이터를 매칭한다. (이벤트 기반 요청-응답 구조)

사용 전 준비:
    pip install pyserial

포트 확인 필수:
    온도센서 아두이노와 ESP32는 서로 다른 COM 포트를 사용한다.
"""

import serial
import csv
import os
from datetime import datetime

TEMP_PORT = 'COM3'       # 온도센서 아두이노 포트
PELTIER_PORT = 'COM4'    # ESP32(펠티어) 포트 - 실제 포트 번호로 수정
BAUD_RATE = 9600

SAVE_DIR = r"C:\Users\hwan9\OneDrive\Desktop\프로젝트\스마트 팩토리\온도센서_csv"
if not os.path.exists(SAVE_DIR):
    os.makedirs(SAVE_DIR)
FILENAME = os.path.join(SAVE_DIR, "온도_펠티어_통합데이터.csv")

sensor_cols = ["기준(1번)", "구간2", "구간3", "구간4", "구간5", "구간6", "구간7"]

try:
    ser_temp = serial.Serial(TEMP_PORT, BAUD_RATE, timeout=1)
    print(f"온도센서 포트({TEMP_PORT}) 연결 성공")
except Exception as e:
    print(f"온도센서 포트 연결 실패: {e}")
    exit()

try:
    ser_peltier = serial.Serial(PELTIER_PORT, BAUD_RATE, timeout=1)
    print(f"펠티어 포트({PELTIER_PORT}) 연결 성공")
except Exception as e:
    print(f"펠티어 포트 연결 실패: {e}")
    exit()

if not os.path.exists(FILENAME):
    with open(FILENAME, mode='w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["순번", "timestamp"] + sensor_cols + ["Delta_T", "펠티어제어값"])

row_count = 0
if os.path.exists(FILENAME):
    with open(FILENAME, mode='r', encoding='utf-8-sig') as f:
        reader = csv.reader(f)
        row_count = sum(1 for row in reader) - 1
if row_count < 0:
    row_count = 0

print(f"저장 위치: {FILENAME}")
print("데이터 수집 시작... (Ctrl+C로 종료)")

try:
    while True:
        if ser_temp.in_waiting > 0:
            line_t = ser_temp.readline().decode('utf-8', errors='ignore').strip()
            try:
                values_t = line_t.split(",")
                if len(values_t) == 7:
                    # 온도값이 들어온 순간, ESP32에 즉시 상태 요청
                    ser_peltier.reset_input_buffer()
                    ser_peltier.write(b'S\n')
                    line_p = ser_peltier.readline().decode('utf-8', errors='ignore').strip()

                    peltier_status = line_p if (len(line_p) == 6 and line_p.isdigit()) else "ERR"

                    temps = [float(v) if v != "ERR" else None for v in values_t]
                    if temps[0] is not None and temps[6] is not None:
                        delta_t = round(temps[6] - temps[0], 2)
                    else:
                        delta_t = None

                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                    row_count += 1

                    with open(FILENAME, mode='a', newline='', encoding='utf-8-sig') as f:
                        writer = csv.writer(f)
                        writer.writerow([row_count, timestamp] + values_t + [delta_t, peltier_status])

                    print(f"[{row_count}] {timestamp} - 온도:{values_t} / Delta_T:{delta_t} / 펠티어:{peltier_status}")
            except ValueError:
                pass

except KeyboardInterrupt:
    print("\n데이터 수집 종료")
    ser_temp.close()
    ser_peltier.close()
