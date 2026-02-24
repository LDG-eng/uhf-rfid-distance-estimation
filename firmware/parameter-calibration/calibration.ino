/*
============================================================
UHF RFID Log-Distance Model Parameter Estimation (Arduino)

Purpose:
Estimate P0 and path-loss exponent (n)
using measured RSSI values at known distances.

Model:
RSSI = P0 − 10n log10(d)

Procedure:
1. Measure average RSSI at reference distance D0 (1m)
   → P0 = average RSSI at 1m
2. Measure average RSSI at D1 (e.g., 3m)
3. Compute:
   n = (P0 − RSSI_D1) / (10 log10(D1 / D0))

The estimated values are copied into the main firmware
for real-time distance estimation.
============================================================
*/


#include <SoftwareSerial.h>
#include <math.h>

// ==================== 보정 설정 ====================
// 평균 계산에 사용할 RSSI 샘플 개수
#define CALIBRATION_SAMPLES   100
// P0 측정을 위한 기준 거리 (미터)
#define D0_METER              1.0f
// n 계산을 위한 두 번째 측정 거리 (미터)
#define D1_METER              3.0f
// ====================================================


// ------------------- Arduino Pin setting ------------------
#define RFID_RX          10
#define RFID_TX          11
SoftwareSerial RFID_Serial(RFID_RX, RFID_TX);
#define Monitor_Serial Serial
#define LED_PIN          13   // 상태 표시용 LED
#define BUTTON_PIN       8
// ----------------------------------------------------------


// ------------------------ 전역 변수 ------------------------
int currentState = 0; // 0: 대기, 1: P0 측정, 2: D1 대기, 3: D1 측정, 4: 계산 및 완료
float p0_value = 0;
// ----------------------------------------------------------

// RFID 읽기 명령
byte ReadMulti[10] = {0xAA, 0x00, 0x27, 0x00, 0x03, 0x22, 0xFF, 0xFF, 0x4A, 0xDD};


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Monitor_Serial.begin(115200);
  RFID_Serial.begin(115200);

  Monitor_Serial.println(F("===== RSSI 모델 파라미터 계산기 ====="));
  Monitor_Serial.println();
  promptForMeasurement(D0_METER);
}

void loop() {
  // 버튼이 눌렸을 때만 다음 단계로 진행
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200); // 디바운싱

    if (currentState == 0) { // P0 측정 시작
      currentState = 1;
      p0_value = measureAverageRssi(D0_METER);
      Monitor_Serial.print(F(" -> 측정된 P0 (1m 평균 RSSI): "));
      Monitor_Serial.println(p0_value, 2);
      Monitor_Serial.println();
      promptForMeasurement(D1_METER);
      currentState = 2;
    }
    else if (currentState == 2) { // D1 측정 시작
      currentState = 3;
      float rssi_at_d1 = measureAverageRssi(D1_METER);
      Monitor_Serial.print(F(" -> "));
      Monitor_Serial.print(D1_METER, 1);
      Monitor_Serial.print(F("m 평균 RSSI: "));
      Monitor_Serial.println(rssi_at_d1, 2);
      Monitor_Serial.println();

      // n 값 계산
      float n_value = (p0_value - rssi_at_d1) / (10.0 * log10(D1_METER / D0_METER));
      
      // 최종 결과 출력
      Monitor_Serial.println(F("===== 최종 계산 결과 ====="));
      Monitor_Serial.print(F("P0: "));
      Monitor_Serial.println(p0_value, 4);
      Monitor_Serial.print(F("n: "));
      Monitor_Serial.println(n_value, 4);
      Monitor_Serial.println(F("=========================="));
      Monitor_Serial.println(F("이 값들을 메인 코드의 P0_FIXED와 N_FIXED에 복사하세요."));
      
      currentState = 4; // 완료 상태
    }
  }

  // 완료 상태일 때 LED 켜기
  if (currentState == 4) {
    digitalWrite(LED_PIN, HIGH);
  } else { // 대기 상태일 때 LED 깜빡이기
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
}

// 다음 측정을 위해 사용자에게 안내 메시지를 출력하는 함수
void promptForMeasurement(float distance) {
  Monitor_Serial.print(F("1. 태그를 "));
  Monitor_Serial.print(distance, 1);
  Monitor_Serial.println(F("m 거리에 위치시키세요."));
  Monitor_Serial.println(F("2. 준비되면 버튼을 눌러 측정을 시작하세요."));
}

// 지정된 거리에서 평균 RSSI를 측정하는 함수
float measureAverageRssi(float distance) {
  Monitor_Serial.print(F("... "));
  Monitor_Serial.print(distance, 1);
  Monitor_Serial.print(F("m 거리에서 "));
  Monitor_Serial.print(CALIBRATION_SAMPLES);
  Monitor_Serial.println(F("개 샘플 수집 중 ..."));
  
  digitalWrite(LED_PIN, HIGH); // 측정 중 LED 켜기

  long rssi_sum = 0;
  int samples_collected = 0;

  // RFID 읽기 명령 전송
  RFID_Serial.write(ReadMulti, 10);
  delay(100); // 모듈이 응답할 시간

  while (samples_collected < CALIBRATION_SAMPLES) {
    if (RFID_Serial.available() > 0) {
      int rssi = getRssiFromStream();
      if (rssi != -1000) { // 유효한 RSSI 값일 경우
        rssi_sum += rssi;
        samples_collected++;
        // 읽기 명령을 계속 보내서 스트림 유지
        if (samples_collected % 10 == 0) {
           RFID_Serial.write(ReadMulti, 10);
        }
      }
    }
  }
  
  digitalWrite(LED_PIN, LOW); // 측정 완료 LED 끄기
  return (float)rssi_sum / samples_collected;
}

// RFID 시리얼 스트림에서 RSSI 값 하나를 파싱하는 함수
int getRssiFromStream() {
  unsigned int d_add = 0, p_state = 0, c_state = 0;
  
  // 타임아웃을 위한 타이머
  unsigned long startTime = millis();
  while(millis() - startTime < 100) { // 100ms 타임아웃
    if(RFID_Serial.available() > 0){
      unsigned int income = RFID_Serial.read();
      if ((income == 0x02) && (p_state == 0)) { p_state = 1; }
      else if ((p_state == 1) && (income == 0x22) && (c_state == 0)) {
        c_state = 1; d_add = 3;
      }
      else if (c_state == 1) {
        d_add++;
        if (d_add == 6) {
          return (income > 127) ? (int)income - 256 : (int)income;
        }
        else if (d_add >= 21) {
          return -1000; // 패킷 끝, 실패
        }
      }
    }
  }
  return -1000; // 타임아웃, 실패
}
