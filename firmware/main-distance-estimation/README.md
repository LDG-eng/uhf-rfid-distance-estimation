/*
 Project: UHF RFID RSSI Distance Estimation
 Hardware: R200 + 5dBi antenna
 Author: LEE-DG
 Description:
   - RSSI-based distance estimation
   - Hybrid RSSI filtering
   - Distance-domain inertia compensation
   - Real-time MAE / RMSE / Accuracy evaluation
*/

#include <SoftwareSerial.h>
#include <math.h>
// ==================== 모델 파라미터 (가장 중요!) ====================
#define P0_FIXED -64.91f
#define N_FIXED 1.769f
#define D0 1.0f
// ==================== 필터 및 테스트 설정 ====================
#define RSSI_MIN_THRESHOLD -85 // 이 값보다 약한 RSSI는 폐기
#define RSSI_MAX_THRESHOLD -35 // 이 값보다 강한 RSSI는 폐기
#define WINDOW_SIZE 5 // 스마트 필터 윈도우 크기
#define OUTLIER_K 1.8f // 이상치 제거 계수 (x * 표준편차)
#define MIX_RATIO 0.4f // 스마트 필터 혼합 비율 (반응성 vs 안정성)
const float positions_to_test[] = {0.1, 0.4, 0.7, 1.0, 1.3, 1.6, 2.0}; //측정할 위치
const int num_positions = sizeof(positions_to_test) / sizeof(float);
// 위치별 관성 계수(Inertia Factor) 배열
const float inertia_factors_per_pos[num_positions] = { 0.6f, // 0.1m (기본값) 0.6f, // 0.4m (기본값) 0.6f, // 0.7m (기본값) 0.5f, // 1.0m (기본값) 0.25f, // 1.3m ( 간섭 구간: 민감하게) 0.25f, // 1.6m ( 간섭 구간: 민감하게) 0.5f // 2.0m (기본값)
};
// 테스트 설정
#define SAMPLES_PER_POSITION 300 // 위치당 수집할 샘플 수
#define SAMPLES_TO_IGNORE 5 // 위치당 초반에 무시할 샘플 수
#define ACCURACY_THRESHOLD_M 0.1f // '성공'으로 판단할 오차 임계값
// ------------------- Arduino Pin setting ------------------ #define RFID_RX 10
#define RFID_TX 11
SoftwareSerial RFID_Serial(RFID_RX, RFID_TX);
#define Monitor_Serial Serial
#define BLUE_LED 13 // 측정 중 LED
#define YELLOW_LED 12 // 대기 중 LED
#define BUTTON_PIN 9 // 9번 핀 스위치
#define BUZER_PIN 8 // 8번 핀 부저
- 26 -
// ------------------------ 전역 변수 ------------------------
int current_pos_index = 0; // 현재 테스트 중인 위치의 인덱스
int samples_at_current_pos = 0; // 현재 위치에서 수집한 샘플 수
int ignore_count = 0; // 현재 위치에서 무시한 샘플 수
int program_state = 0; // 0: 대기, 1: 측정 중
// 필터링용 윈도우 및 상태 변수
float rssi_window[WINDOW_SIZE];
int window_count = 0;
float prev_filt_dist = 0.0;
bool is_first_run = true;
// 통계용 변수 (위치별로 분리)
float raw_dist_sum_abs_error_per_pos[num_positions] = {0};
float filt_dist_sum_abs_error_per_pos[num_positions] = {0};
float raw_dist_sum_sq_error_per_pos[num_positions] = {0};
float filt_dist_sum_sq_error_per_pos[num_positions] = {0};
long total_reading_count_per_pos[num_positions] = {0};
long raw_success_count_per_pos[num_positions] = {0};
long filt_success_count_per_pos[num_positions] = {0};
// ---------------------------------------------------------- byte ReadMulti[10] = {0xAA, 0x00, 0x27, 0x00, 0x03, 0x22, 0xFF, 0xFF, 0x4A, 0xDD};
void setup() { pinMode(BLUE_LED, OUTPUT); pinMode(YELLOW_LED, OUTPUT); pinMode(BUZER_PIN, OUTPUT); pinMode(BUTTON_PIN, INPUT_PULLUP);
 Monitor_Serial.begin(115200); RFID_Serial.begin(115200);
 digitalWrite(YELLOW_LED, HIGH); digitalWrite(BLUE_LED, LOW);
 promptForNextPosition();
}
void loop() { // --- 0. 측정 대기 상태 ---
 if (program_state == 0) { digitalWrite(YELLOW_LED, HIGH); digitalWrite(BLUE_LED, LOW);
 // 버튼 입력으로 측정 시작
 if (digitalRead(BUTTON_PIN) == LOW) { delay(200);
 program_state = 1;

 samples_at_current_pos = 0;
- 27 -
 ignore_count = 0; is_first_run = true; prime_filter_window();

 Monitor_Serial.println(F("...측정 시작...")); tone(BUZER_PIN, 1000, 150); // 시작음
 RFID_Serial.write(ReadMulti, 10);
 } } // --- 1. 측정 진행 상태 ---
 else if (program_state == 1) { digitalWrite(YELLOW_LED, LOW); digitalWrite(BLUE_LED, HIGH);
 if (RFID_Serial.available() > 0) { int rssi_dbm = getRssiFromStream(); if (rssi_dbm != -1000) { processAndAnalyze(rssi_dbm); RFID_Serial.write(ReadMulti, 10); // 스트림 유지
 } }
 // 현재 위치의 샘플 수집이 완료되면
 if (samples_at_current_pos >= SAMPLES_PER_POSITION) { tone(BUZER_PIN, 1200, 150); // 종료음
 Monitor_Serial.print(positions_to_test[current_pos_index], 1); Monitor_Serial.println(F("m 위치 측정 완료.\n"));
 current_pos_index++; // 다음 위치 인덱스로

 // 모든 위치 측정이 끝났다면
 if (current_pos_index >= num_positions) { displayFinalReport(); // 최종 리포트 출력
 digitalWrite(BLUE_LED, LOW); digitalWrite(YELLOW_LED, HIGH); // 완료 LED
 while (1); // 프로그램 정지
 }
 // 다음 측정을 위해 대기 상태로 전환
 program_state = 0;
 promptForNextPosition(); } }
}
// -------------------- 핵심 분석 및 처리 함수 -------------------- void processAndAnalyze(int raw_rssi) { // 1. 초기 샘플 무시
 if (ignore_count < SAMPLES_TO_IGNORE) { ignore_count++;
- 28 -
 return; // 통계에 포함하지 않고 함수 종료
 } // 통계 카운트 증가
 samples_at_current_pos++;
 total_reading_count_per_pos[current_pos_index]++; float real_pos = positions_to_test[current_pos_index];
 // --- 2. Raw 데이터 분석 (필터 없음) ---
 float raw_dist = rssiToDistance(raw_rssi); float raw_error = raw_dist - real_pos; raw_dist_sum_abs_error_per_pos[current_pos_index] += fabs(raw_error); raw_dist_sum_sq_error_per_pos[current_pos_index] += raw_error * raw_error; if (fabs(raw_error) <= ACCURACY_THRESHOLD_M) { raw_success_count_per_pos[current_pos_index]++; }
 // --- 3. Filtered 데이터 분석 ---
 float final_filt_dist = -1.0;
 String filter_status = "";
 // 3a. 물리적 한계 필터링
 if (raw_rssi < RSSI_MIN_THRESHOLD || raw_rssi > RSSI_MAX_THRESHOLD) { filter_status = "(Raw 폐기)"; } else { // 3b. 스마트 필터 적용 (윈도우 업데이트, 이상치 제거 등) float filtered_rssi = apply_smart_filter(raw_rssi);
 // 3c. RSSI -> 거리 변환
 float filt_dist_no_inertia = rssiToDistance(filtered_rssi);
 if (is_first_run) { prev_filt_dist = filt_dist_no_inertia; is_first_run = false; }
 // (변경) 3d. 관성 필터 적용 (위치별 관성 계수 사용) float current_inertia = inertia_factors_per_pos[current_pos_index]; final_filt_dist = (current_inertia * prev_filt_dist) + ((1.0f - current_inertia) * filt_dist_no_inertia); prev_filt_dist = final_filt_dist;
 // 3e. 필터링된 거리로 오차 계산
 float filt_error = final_filt_dist - real_pos; filt_dist_sum_abs_error_per_pos[current_pos_index] += fabs(filt_error); filt_dist_sum_sq_error_per_pos[current_pos_index] += filt_error * filt_error; if (fabs(filt_error) <= ACCURACY_THRESHOLD_M) { filt_success_count_per_pos[current_pos_index]++; } }
 // --- 4. 결과 출력 (실시간 모니터링용) ---
 String data_str = "RSSI : " + String(raw_rssi) + "dBm, 실제: " + String(real_pos, 1) + "m" +
- 29 -
 " -> Raw : " + String(raw_dist, 2) + "m" +
 " / Filtered : "; if (final_filt_dist != -1.0) { data_str += String(final_filt_dist, 2) + "m"; } else { data_str += filter_status; } Monitor_Serial.println(data_str);
}
// -------------------- 유틸리티 및 리포트 함수 --------------------
// 필터 윈도우를 현재 위치의 이론적 RSSI 값으로 채우는 함수
void prime_filter_window() { float real_pos = positions_to_test[current_pos_index]; float theoretical_rssi = theoreticalRssi(real_pos); for (int i = 0; i < WINDOW_SIZE; i++) { rssi_window[i] = theoretical_rssi; } window_count = WINDOW_SIZE;
}
// RFID 스트림에서 RSSI 값 하나를 파싱하는 함수
int getRssiFromStream() { unsigned int d_add = 0, p_state = 0, c_state = 0; unsigned long startTime = millis(); while(millis() - startTime < 100) { // 100ms 타임아웃
 if(RFID_Serial.available() > 0){ unsigned int income = RFID_Serial.read(); if ((income == 0x02) && (p_state == 0)) { p_state = 1; } else if ((p_state == 1) && (income == 0x22) && (c_state == 0)) { c_state = 1; d_add = 3; } else if (c_state == 1) { d_add++; if (d_add == 6) { return (income > 127) ? (int)income - 256 : (int)income; } else if (d_add >= 21) { return -1000; } } } } return -1000; // 타임아웃, 실패
}
// (모델 공식) RSSI -> 거리
float rssiToDistance(float rssi) { // 거리(m) = 10 ^ ((P0 - RSSI) / (10 * n)) return pow(10.0f, (P0_FIXED - rssi) / (10.0f * N_FIXED));
}
// (모델 공식) 거리 -> 이론적 RSSI
float theoreticalRssi(float distance) {
- 30 -
 if (distance == 0) distance = 0.01; // 0 방지
 // RSSI = P0 - 10 * n * log10(거리 / D0) return P0_FIXED - 10.0f * N_FIXED * log10f(distance / D0);
}
// Serial.print()를 사용하도록 최종 리포트 함수
void displayFinalReport() { Monitor_Serial.println(F("\n\n===== 최종 모델 성능 보고서 (상세) =====")); Monitor_Serial.print(F("모델 P0: ")); Monitor_Serial.print(P0_FIXED, 2); Monitor_Serial.print(F(" | n: ")); Monitor_Serial.println(N_FIXED, 3); Monitor_Serial.print(F("정확도 기준: 오차 ")); Monitor_Serial.print(ACCURACY_THRESHOLD_M);
Monitor_Serial.println(F("m 이내"));
 long total_readings = 0; long total_raw_success = 0; long total_filt_success = 0; float total_raw_sum_abs_err = 0; float total_filt_sum_abs_err = 0; float total_raw_sum_sq_err = 0; float total_filt_sum_sq_err = 0;
 Monitor_Serial.println(F("\n--- [위치별 상세 성능] ---")); Monitor_Serial.println(F("위치 | Raw MAE | Filt MAE | Raw RMSE | Filt RMSE | Raw Acc(%) | Filt Acc(%)")); Monitor_Serial.println(F("-----------------------------------------------------------------------------"));
 for (int i = 0; i < num_positions; i++) { long count = total_reading_count_per_pos[i]; if (count == 0) continue;
 // 총합 계산
 total_readings += count; total_raw_success += raw_success_count_per_pos[i]; total_filt_success += filt_success_count_per_pos[i]; total_raw_sum_abs_err += raw_dist_sum_abs_error_per_pos[i]; total_filt_sum_abs_err += filt_dist_sum_abs_error_per_pos[i]; total_raw_sum_sq_err += raw_dist_sum_sq_error_per_pos[i]; total_filt_sum_sq_err += filt_dist_sum_sq_error_per_pos[i];
 // 위치별 통계 계산
 float raw_mae = raw_dist_sum_abs_error_per_pos[i] / count; float filt_mae = filt_dist_sum_abs_error_per_pos[i] / count; float raw_rmse = sqrt(raw_dist_sum_sq_error_per_pos[i] / count); float filt_rmse = sqrt(filt_dist_sum_sq_error_per_pos[i] / count); float raw_accuracy = (float)raw_success_count_per_pos[i] / count * 100.0f; float filt_accuracy = (float)filt_success_count_per_pos[i] / count * 100.0f;
 // 수동 출력
 Monitor_Serial.print(positions_to_test[i], 1); Monitor_Serial.print(F("m | ")); Monitor_Serial.print(raw_mae, 3); Monitor_Serial.print(F(" | ")); Monitor_Serial.print(filt_mae, 3); Monitor_Serial.print(F(" | ")); Monitor_Serial.print(raw_rmse, 3); Monitor_Serial.print(F(" | "));
- 31 -
 Monitor_Serial.print(filt_rmse, 3); Monitor_Serial.print(F(" | ")); Monitor_Serial.print(raw_accuracy, 1); Monitor_Serial.print(F(" | ")); Monitor_Serial.println(filt_accuracy, 1); }
 if (total_readings == 0) { Monitor_Serial.println(F("측정 데이터가 없습니다.")); return; } Monitor_Serial.println(F("-----------------------------------------------------------------------------"));
 // 전체 통계 계산
 float total_raw_mae = total_raw_sum_abs_err / total_readings; float total_filt_mae = total_filt_sum_abs_err / total_readings; float total_raw_rmse = sqrt(total_raw_sum_sq_err / total_readings); float total_filt_rmse = sqrt(total_filt_sum_sq_err / total_readings); float total_raw_accuracy = (float)total_raw_success / total_readings * 100.0f; float total_filt_accuracy = (float)total_filt_success / total_readings * 100.0f;
 Monitor_Serial.println(F("\n--- [전체 종합 성능] ---")); Monitor_Serial.println(F("구분 | MAE | RMSE | Accuracy(%)")); Monitor_Serial.println(F("-------------------------------------------------"));
 // 수동 출력
 Monitor_Serial.print(F("Raw | ")); Monitor_Serial.print(total_raw_mae, 3); Monitor_Serial.print(F("m | ")); Monitor_Serial.print(total_raw_rmse, 3); Monitor_Serial.print(F("m | ")); Monitor_Serial.print(total_raw_accuracy, 1); Monitor_Serial.println(F("%"));
 Monitor_Serial.print(F("Filtered | ")); Monitor_Serial.print(total_filt_mae, 3); Monitor_Serial.print(F("m | ")); Monitor_Serial.print(total_filt_rmse, 3); Monitor_Serial.print(F("m | ")); Monitor_Serial.print(total_filt_accuracy, 1); Monitor_Serial.println(F("%"));
 Monitor_Serial.println(F("================================================="));
}
// 안내 문구
void promptForNextPosition() { float pos_to_setup = positions_to_test[current_pos_index]; Monitor_Serial.println(); Monitor_Serial.print(F("1. 태그를 [ ")); Monitor_Serial.print(pos_to_setup, 1); Monitor_Serial.println(F("m ] 거리에 위치시키세요.")); Monitor_Serial.println(F("2. 준비되면 버튼을 눌러 측정을 시작하세요."));
}
// 스마트 필터 (윈도우 업데이트, 이상치 제거, 혼합 평균)
float apply_smart_filter(int new_rssi) { // 1. 윈도우 업데이트 (Sliding Window) for (int j = 0; j < WINDOW_SIZE - 1; j++) {
- 32 -
 rssi_window[j] = rssi_window[j + 1]; } rssi_window[WINDOW_SIZE - 1] = (float)new_rssi;
 // 2. 통계적 이상치 제거 (Outlier Rejection) float m = 0, s = 0; for(int i=0; i<WINDOW_SIZE; i++) m += rssi_window[i]; m /= WINDOW_SIZE; for(int i=0; i<WINDOW_SIZE; i++) s += pow(rssi_window[i] - m, 2); s = sqrt(s / WINDOW_SIZE);
 float clean_window[WINDOW_SIZE]; int clean_size = 0; if (s > 0.01) { // 표준편차가 0에 가까우면(모든 값이 같으면) 스킵
 for (int i = 0; i < WINDOW_SIZE; i++) { if (fabs(rssi_window[i] - m) <= OUTLIER_K * s) { clean_window[clean_size++] = rssi_window[i]; } } } else { for (int i = 0; i < WINDOW_SIZE; i++) clean_window[i] = rssi_window[i]; clean_size = WINDOW_SIZE; }
 if (clean_size == 0) return m; // 모든 값이 이상치면 그냥 평균 반환
 // 3. 적응형 혼합 (Adaptive Mixing) float simple_avg = 0; for(int i=0; i<clean_size; i++) simple_avg += clean_window[i]; simple_avg /= clean_size;
 float weighted_sum = 0, weight_total = 0; for (int i = 0; i < clean_size; i++) { float weight = (float)(i + 1); // 오래된 값에 낮은 가중치, 새 값에 높은 가중치
 weighted_sum += clean_window[i] * weight; weight_total += weight; } float weighted_avg = (weight_total > 0) ? weighted_sum / weight_total : simple_avg;
 // 4. 최종 값 반환 (단순 평균과 가중 평균의 혼합) return MIX_RATIO * weighted_avg + (1.0f - MIX_RATIO) * simple_avg;
}
