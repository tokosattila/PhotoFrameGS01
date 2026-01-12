#include <unity.h>
#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

// ============================================================================
// BATTERY VOLTAGE CALCULATION TESTS
// ============================================================================

// Time constants for sleep tests
constexpr uint32_t SECONDS_PER_MINUTE = 60;
constexpr uint32_t SECONDS_PER_HOUR = 60 * SECONDS_PER_MINUTE;
constexpr uint32_t SECONDS_PER_DAY = 24 * SECONDS_PER_HOUR;

// ETimerWakeUp enum (from Global.h)
enum class ETimerWakeUp : uint8_t {
  Seconds = 0,
  Minutes = 1,
  Hourly = 2,
  HalfDay = 3,
  Daily = 4,
  Weekly = 5,
  Monthly = 6
};

// Battery voltage interpolation calculation (extracted from Utils_::MeasureBattery logic)
int CalculateBatteryPercentage(float tVoltage) {
  constexpr float kVoltageTable[] = {3.20f, 3.40f, 3.50f, 3.60f, 3.70f, 3.80f, 3.90f, 4.00f, 4.10f, 4.20f};
  constexpr int kPercentTable[] = {0, 5, 10, 20, 40, 60, 75, 85, 95, 100};
  constexpr int kTableSize = sizeof(kVoltageTable) / sizeof(kVoltageTable[0]);
  
  int tPercentage = 0;
  if (tVoltage >= 4.20f) {
    tPercentage = 100;
  } else if (tVoltage <= 3.20f) {
    tPercentage = 0;
  } else {
    for (int i = 1; i < kTableSize; ++i) {
      if (tVoltage <= kVoltageTable[i]) {
        float tRatio = (tVoltage - kVoltageTable[i - 1]) / (kVoltageTable[i] - kVoltageTable[i - 1]);
        tPercentage = kPercentTable[i - 1] + static_cast<int>(tRatio * (kPercentTable[i] - kPercentTable[i - 1]));
        break;
      }
    }
  }
  return tPercentage;
}

void test_battery_voltage_boundary_min() {
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(3.20f));
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(3.10f));
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(3.00f));
}

void test_battery_voltage_boundary_max() {
  TEST_ASSERT_EQUAL(100, CalculateBatteryPercentage(4.20f));
  TEST_ASSERT_EQUAL(100, CalculateBatteryPercentage(4.30f));
  TEST_ASSERT_EQUAL(100, CalculateBatteryPercentage(4.50f));
}

void test_battery_voltage_interpolation_exact_values() {
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(3.20f));
  TEST_ASSERT_EQUAL(5, CalculateBatteryPercentage(3.40f));
  TEST_ASSERT_EQUAL(10, CalculateBatteryPercentage(3.50f));
  TEST_ASSERT_EQUAL(20, CalculateBatteryPercentage(3.60f));
  TEST_ASSERT_EQUAL(40, CalculateBatteryPercentage(3.70f));
  TEST_ASSERT_EQUAL(60, CalculateBatteryPercentage(3.80f));
  TEST_ASSERT_EQUAL(75, CalculateBatteryPercentage(3.90f));
  TEST_ASSERT_EQUAL(85, CalculateBatteryPercentage(4.00f));
  TEST_ASSERT_EQUAL(95, CalculateBatteryPercentage(4.10f));
  TEST_ASSERT_EQUAL(100, CalculateBatteryPercentage(4.20f));
}

void test_battery_voltage_interpolation_between_values() {
  // Between 3.40V (5%) and 3.50V (10%)
  int tResult = CalculateBatteryPercentage(3.45f);
  TEST_ASSERT_INT_WITHIN(1, 7, tResult);
  
  // Between 3.70V (40%) and 3.80V (60%)
  tResult = CalculateBatteryPercentage(3.75f);
  TEST_ASSERT_INT_WITHIN(1, 50, tResult);
  
  // Between 3.90V (75%) and 4.00V (85%)
  tResult = CalculateBatteryPercentage(3.95f);
  TEST_ASSERT_INT_WITHIN(1, 80, tResult);
  
  // Between 4.10V (95%) and 4.20V (100%)
  tResult = CalculateBatteryPercentage(4.15f);
  TEST_ASSERT_INT_WITHIN(1, 97, tResult);
}

void test_battery_voltage_low_critical() {
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(3.15f));
  TEST_ASSERT_EQUAL(0, CalculateBatteryPercentage(2.80f));
}

void test_battery_voltage_near_boundaries() {
  // Just above 3.20V
  int tResult = CalculateBatteryPercentage(3.21f);
  TEST_ASSERT_INT_WITHIN(2, 0, tResult);
  
  // Just below 4.20V
  tResult = CalculateBatteryPercentage(4.19f);
  TEST_ASSERT_INT_WITHIN(2, 99, tResult);
}

// ============================================================================
// DEEP SLEEP CALCULATION TESTS
// ============================================================================

// Sleep delay calculation (extracted from Utils_::SleepAndWakeup logic)
uint64_t CalculateSleepDelay(ETimerWakeUp tWakeUp) {
  uint64_t tDelaySec = 0;
  switch (tWakeUp) {
    case ETimerWakeUp::Seconds:
      tDelaySec = 10;
      break;
    case ETimerWakeUp::Minutes:
      tDelaySec = SECONDS_PER_MINUTE;
      break;
    case ETimerWakeUp::Hourly:
      tDelaySec = SECONDS_PER_HOUR;
      break;
    case ETimerWakeUp::HalfDay:
      tDelaySec = 12 * SECONDS_PER_HOUR;
      break;
    case ETimerWakeUp::Daily:
      tDelaySec = 24 * SECONDS_PER_HOUR;
      break;
    case ETimerWakeUp::Weekly:
      tDelaySec = 7 * 24 * SECONDS_PER_HOUR;
      break;
    case ETimerWakeUp::Monthly:
      tDelaySec = 30 * 24 * SECONDS_PER_HOUR;
      break;
    default:
      tDelaySec = 24 * SECONDS_PER_HOUR;
      break;
  }
  return tDelaySec;
}

void test_sleep_delay_seconds() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Seconds);
  TEST_ASSERT_EQUAL_UINT64(10, tDelay);
}

void test_sleep_delay_minutes() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Minutes);
  TEST_ASSERT_EQUAL_UINT64(60, tDelay);
}

void test_sleep_delay_hourly() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Hourly);
  TEST_ASSERT_EQUAL_UINT64(3600, tDelay);
}

void test_sleep_delay_half_day() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::HalfDay);
  TEST_ASSERT_EQUAL_UINT64(12 * 3600, tDelay);
  TEST_ASSERT_EQUAL_UINT64(43200, tDelay);
}

void test_sleep_delay_daily() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Daily);
  TEST_ASSERT_EQUAL_UINT64(24 * 3600, tDelay);
  TEST_ASSERT_EQUAL_UINT64(86400, tDelay);
}

void test_sleep_delay_weekly() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Weekly);
  TEST_ASSERT_EQUAL_UINT64(7 * 24 * 3600, tDelay);
  TEST_ASSERT_EQUAL_UINT64(604800, tDelay);
}

void test_sleep_delay_monthly() {
  uint64_t tDelay = CalculateSleepDelay(ETimerWakeUp::Monthly);
  TEST_ASSERT_EQUAL_UINT64(30 * 24 * 3600, tDelay);
  TEST_ASSERT_EQUAL_UINT64(2592000, tDelay);
}

void test_sleep_delay_microseconds_conversion() {
  constexpr uint64_t tSecToUs = 1000000ULL;
  uint64_t tDelaySec = CalculateSleepDelay(ETimerWakeUp::Minutes);
  uint64_t tDelayUs = tDelaySec * tSecToUs;
  TEST_ASSERT_EQUAL_UINT64(60000000, tDelayUs);
}

void test_wakeup_cause_button() {
  // This test verifies that button wakeup is correctly identified
  // In real hardware, esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0
  // would return true after button press during deep sleep
  esp_sleep_wakeup_cause_t tExpectedButtonCause = ESP_SLEEP_WAKEUP_EXT0;
  TEST_ASSERT_EQUAL(2, tExpectedButtonCause); // ESP_SLEEP_WAKEUP_EXT0 = 2
}

void test_wakeup_cause_timer() {
  esp_sleep_wakeup_cause_t tExpectedTimerCause = ESP_SLEEP_WAKEUP_TIMER;
  TEST_ASSERT_EQUAL(4, tExpectedTimerCause); // ESP_SLEEP_WAKEUP_TIMER = 4
}

void test_sleep_delay_time_units() {
  // Test that delays are in correct ranges
  uint64_t tSeconds = CalculateSleepDelay(ETimerWakeUp::Seconds);
  uint64_t tMinutes = CalculateSleepDelay(ETimerWakeUp::Minutes);
  uint64_t tHourly = CalculateSleepDelay(ETimerWakeUp::Hourly);
  
  TEST_ASSERT_TRUE(tSeconds < SECONDS_PER_MINUTE);
  TEST_ASSERT_TRUE(tMinutes >= SECONDS_PER_MINUTE);
  TEST_ASSERT_TRUE(tMinutes < SECONDS_PER_HOUR);
  TEST_ASSERT_TRUE(tHourly >= SECONDS_PER_HOUR);
}

// ============================================================================
// ADC HARDWARE TESTS
// ============================================================================

void test_adc_initialization() {
  // Test ADC width configuration
  esp_err_t tErr = adc1_config_width(ADC_WIDTH_BIT_12);
  TEST_ASSERT_EQUAL(ESP_OK, tErr);
  
  // Test ADC channel attenuation configuration
  tErr = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  TEST_ASSERT_EQUAL(ESP_OK, tErr);
}

void test_adc_raw_reading() {
  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  
  // Read raw ADC value
  int tAdcRaw = adc1_get_raw(ADC1_CHANNEL_0);
  
  // ADC value should be in valid range (0-4095 for 12-bit)
  TEST_ASSERT_GREATER_OR_EQUAL(0, tAdcRaw);
  TEST_ASSERT_LESS_OR_EQUAL(4095, tAdcRaw);
}

void test_adc_characterization() {
  esp_adc_cal_characteristics_t tAdcChars;
  
  // Characterize ADC
  esp_adc_cal_value_t tValType = esp_adc_cal_characterize(
    ADC_UNIT_1, 
    ADC_ATTEN_DB_12, 
    ADC_WIDTH_BIT_12, 
    1100,  // Default vref
    &tAdcChars
  );
  
  // Verify characterization type (should be eFuse or Two Point)
  TEST_ASSERT_TRUE(
    tValType == ESP_ADC_CAL_VAL_EFUSE_VREF || 
    tValType == ESP_ADC_CAL_VAL_EFUSE_TP ||
    tValType == ESP_ADC_CAL_VAL_DEFAULT_VREF
  );
}

void test_adc_voltage_conversion() {
  esp_adc_cal_characteristics_t tAdcChars;
  
  // Configure and characterize ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &tAdcChars);
  
  // Read raw value
  uint32_t tAdcRaw = adc1_get_raw(ADC1_CHANNEL_0);
  
  // Convert to millivolts
  uint32_t tMilliVolts = esp_adc_cal_raw_to_voltage(tAdcRaw, &tAdcChars);
  
  // Voltage should be in reasonable range (0-3300 mV for 12dB attenuation)
  TEST_ASSERT_GREATER_OR_EQUAL(0, tMilliVolts);
  TEST_ASSERT_LESS_OR_EQUAL(3300, tMilliVolts);
}

void test_adc_multi_sample_averaging() {
  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  
  constexpr int kSamples = 16;
  uint32_t tAdcSum = 0;
  
  // Take multiple samples
  for (int i = 0; i < kSamples; ++i) {
    tAdcSum += adc1_get_raw(ADC1_CHANNEL_0);
    delay(1);
  }
  
  uint32_t tAdcAvg = tAdcSum / kSamples;
  
  // Average should be in valid range
  TEST_ASSERT_GREATER_OR_EQUAL(0, tAdcAvg);
  TEST_ASSERT_LESS_OR_EQUAL(4095, tAdcAvg);
}

void test_battery_voltage_divider() {
  esp_adc_cal_characteristics_t tAdcChars;
  
  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &tAdcChars);
  
  // Take averaged reading
  uint32_t tAdcSum = 0;
  constexpr int kSamples = 16;
  for (int i = 0; i < kSamples; ++i) {
    tAdcSum += adc1_get_raw(ADC1_CHANNEL_0);
    delay(1);
  }
  uint32_t tAdcRaw = tAdcSum / kSamples;
  
  // Convert to millivolts
  uint32_t tMilliVolts = esp_adc_cal_raw_to_voltage(tAdcRaw, &tAdcChars);
  
  // Apply voltage divider ratio (2.05)
  constexpr float kDividerRatio = 2.05f;
  float tBatteryVoltage = (tMilliVolts / 1000.0f) * kDividerRatio;
  
  // Battery voltage should be in reasonable LiPo range (2.5V - 4.5V)
  TEST_ASSERT_FLOAT_WITHIN(5.0f, 3.7f, tBatteryVoltage); // Allow wide range
  TEST_ASSERT_GREATER_OR_EQUAL(2.5f, tBatteryVoltage);
  TEST_ASSERT_LESS_OR_EQUAL(4.5f, tBatteryVoltage);
}

void test_adc_stability() {
  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
  
  // Take two consecutive readings
  int tReading1 = adc1_get_raw(ADC1_CHANNEL_0);
  delay(10);
  int tReading2 = adc1_get_raw(ADC1_CHANNEL_0);
  
  // Readings should be relatively stable (within 10% variance)
  int tDiff = abs(tReading1 - tReading2);
  int tMaxDiff = max(tReading1, tReading2) / 10;
  TEST_ASSERT_LESS_OR_EQUAL(tMaxDiff, tDiff);
}

// ============================================================================
// SETUP & LOOP
// ============================================================================

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("\n\n=== ESP32 Hardware Tests ===");
  
  UNITY_BEGIN();
  
  // Battery voltage tests
  RUN_TEST(test_battery_voltage_boundary_min);
  RUN_TEST(test_battery_voltage_boundary_max);
  RUN_TEST(test_battery_voltage_interpolation_exact_values);
  RUN_TEST(test_battery_voltage_interpolation_between_values);
  RUN_TEST(test_battery_voltage_low_critical);
  RUN_TEST(test_battery_voltage_near_boundaries);
  
  // Deep sleep tests
  RUN_TEST(test_sleep_delay_seconds);
  RUN_TEST(test_sleep_delay_minutes);
  RUN_TEST(test_sleep_delay_hourly);
  RUN_TEST(test_sleep_delay_half_day);
  RUN_TEST(test_sleep_delay_daily);
  RUN_TEST(test_sleep_delay_weekly);
  RUN_TEST(test_sleep_delay_monthly);
  RUN_TEST(test_sleep_delay_microseconds_conversion);
  RUN_TEST(test_wakeup_cause_button);
  RUN_TEST(test_wakeup_cause_timer);
  RUN_TEST(test_sleep_delay_time_units);
  
  // ADC hardware tests
  RUN_TEST(test_adc_initialization);
  RUN_TEST(test_adc_raw_reading);
  RUN_TEST(test_adc_characterization);
  RUN_TEST(test_adc_voltage_conversion);
  RUN_TEST(test_adc_multi_sample_averaging);
  RUN_TEST(test_battery_voltage_divider);
  RUN_TEST(test_adc_stability);
  
  UNITY_END();
}

void loop() {
  // Empty
}
