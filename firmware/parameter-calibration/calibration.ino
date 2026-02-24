/*
============================================================
[UHF RFID Log-Distance Model Data Collector]

Description:
Collects RSSI samples at predefined distances to estimate 
path-loss parameters (P0, n) for environment-specific calibration.

Hardware Interface:
- Button (Pin 9): Trigger measurement start
- Blue LED (Pin 13): Measuring status
- Yellow LED (Pin 12): Idle/Wait status
- Buzzer (Pin 8): Start/Finish notification

Calibration Formula:
RSSI = P0 - 10 * n * log10(d)

Workflow:
1. Place tag at target distance (0.1m to 2.0m).
2. Press button to collect 300 samples.
3. Use averaged results to calculate P0 and n via Least Squares Method.
============================================================
*/


#include <SoftwareSerial.h>
#include <math.h>

// ==================== 설정 (Settings) ====================
// 1. 데이터를 수집할 거리 목록 (미터 단위)
const float positions_to_collect[] = {0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 2.0};

// 2. 한 위치 당 수집할 RSSI 샘플 개수
#define SAMPLES_PER_POSITION 300
// =======================================================


// ------------------- Arduino Pin setting ------------------
// ⭐⭐ 요청사항 반영: 핀 정의 수정 및 추가 ⭐⭐
#define RFID_RX           10
#define RFID_TX           11
SoftwareSerial RFID_Serial(RFID_RX, RFID_TX);
#define Monitor_Serial Serial
#define BLUE_LED          13  // 측정 중 LED
#define YELLOW_LED        12  // 대기 중 LED
#define BUTTON_PIN        9   // 9번 핀 스위치
#define BUZZER_PIN        8   // 8번 핀 부저
// ----------------------------------------------------------


// ------------------------ 전역 변수 ------------------------
const int num_positions = sizeof(positions_to_collect) / sizeof(float);
int current_pos_index = 0;
int program_state = 0; // 0: 대기, 1: 측정 중

long reading_count = 0;
float rssi_sum = 0;
// ----------------------------------------------------------

byte ReadMulti[10] = {0xAA, 0x00, 0x27, 0x00, 0x03, 0.22, 0xFF, 0xFF, 0x4A, 0xDD};


void setup() {
  // ⭐⭐ 요청사항 반영: 핀 모드 설정 ⭐⭐
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 9번 핀 버튼
  
  Monitor_Serial.begin(115200);
  RFID_Serial.begin(115200);

  Monitor_Serial.println(F("\n===== 새 환경 기준 데이터 수집기 (버튼) ====="));
  Monitor_Serial.println(F("측정 범위: 0.1m ~ 1.9m"));
  Monitor_Serial.print(F("위치당 샘플 수: "));
  Monitor_Serial.println(SAMPLES_PER_POSITION);
  
  // 초기 LED 상태 설정: 대기(노란색 ON, 파란색 OFF)
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
  
  promptForNextPosition(); 
}

void loop() {
  if (program_state == 0) { // 대기 상태
    // ⭐⭐ 요청사항 반영: 대기 LED 켜기 ⭐⭐
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);

    // ⭐⭐ 요청사항 반영: 9번 핀 버튼으로 측정 시작 ⭐⭐
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(200); // 디바운싱
      startMeasurement();
    }
  } 
  else if (program_state == 1) { // 측정 상태
    // ⭐⭐ 요청사항 반영: 측정 중 LED 켜기 ⭐⭐
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);

    if (RFID_Serial.available() > 0) {
      int rssi_dbm = getRssiFromStream();
      if (rssi_dbm != -1000) {
        rssi_sum += rssi_dbm;
        reading_count++;
        RFID_Serial.write(ReadMulti, 10); 
      }
    }

    if (reading_count >= SAMPLES_PER_POSITION) {
      finishMeasurement();
    }
  }
}

void startMeasurement() {
  program_state = 1; 
  reading_count = 0;
  rssi_sum = 0;
  
  Monitor_Serial.print(F("... "));
  Monitor_Serial.print(positions_to_collect[current_pos_index], 1);
  Monitor_Serial.print(F("m 측정 시작 ("));
  Monitor_Serial.print(SAMPLES_PER_POSITION);
  Monitor_Serial.println(F("개 수집) ..."));
  
  // ⭐⭐ 요청사항 반영: 측정 시작 부저음 ⭐⭐
  tone(BUZZER_PIN, 1000, 150);

  RFID_Serial.write(ReadMulti, 10);
}

void finishMeasurement() {
  // ⭐⭐ 요청사항 반영: 측정 종료 부저음 ⭐⭐
  tone(BUZZER_PIN, 1200, 150);
  
  if (reading_count > 0) {
    float avg_rssi = rssi_sum / reading_count;
    Monitor_Serial.print(F("결과: "));
    Monitor_Serial.print(reading_count);
    Monitor_Serial.print(F("개 수신, 평균 RSSI: "));
    Monitor_Serial.println(avg_rssi, 2);
  } else {
    Monitor_Serial.println(F("오류: 유효한 태그를 읽지 못했습니다."));
  }

  current_pos_index++;
  if (current_pos_index >= num_positions) {
    displayFinalReport();
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH); // 완료 후 노란 LED 켜기
    while(1); 
  } else {
    program_state = 0;
    promptForNextPosition();
  }
}

void displayFinalReport() {
    Monitor_Serial.println(F("\n\n============================================="));
    Monitor_Serial.println(F("  모든 데이터 수집이 완료되었습니다."));
    Monitor_Serial.println(F("  측정 결과를 엑셀 등으로 정리하여"));
    Monitor_Serial.println(F("  분석(중앙값 계산)을 요청해주세요."));
    Monitor_Serial.println(F("============================================="));
}


void promptForNextPosition() {
  float pos_to_setup = positions_to_collect[current_pos_index];
  Monitor_Serial.println();
  Monitor_Serial.print(F("1. 태그를 [ "));
  Monitor_Serial.print(pos_to_setup, 1);
  Monitor_Serial.println(F("m ] 거리에 위치시키세요."));
  // ⭐⭐ 요청사항 반영: 안내 메시지 수정 ⭐⭐
  Monitor_Serial.println(F("2. 준비되면 버튼을 눌러 측정을 시작하세요."));
}

int getRssiFromStream() {
  unsigned int d_add = 0, p_state = 0, c_state = 0;
  unsigned long startTime = millis();
  while(millis() - startTime < 100) {
    if(RFID_Serial.available() > 0){
      unsigned int income = RFID_Serial.read();
      if ((income == 0x02) && (p_state == 0)) { p_state = 1; }
      else if ((p_state == 1) && (income == 0x22) && (c_state == 0)) {
        c_state = 1; d_add = 3;
      }
      else if (c_state == 1) {
        d_add++;
        if (d_add == 6) { return (income > 127) ? (int)income - 256 : (int)income; }
        else if (d_add >= 21) { return -1000; }
      }
    }
  }
  return -1000;
}
