/*
 * Teensy 4.1 oven characterization + SSR burst control + reflow curve tester
 * Based on Ryan's A10/A11 thermocouple and A12 NTC SD logger.
 *
 * Arduino IDE: Teensy 4.1, USB Type Serial, Serial Monitor 115200, Newline.
 * Uses Teensyduino's bundled SD, SPI and IntervalTimer; no PID library needed.
 *
 * LOW-VOLTAGE CONNECTIONS (verify your actual interface):
 *   A10: amplified thermocouple through 10k/10k divider
 *   A11: amplifier bias through 10k/10k divider
 *   A12: 3.3 V -- fixed 10k -- A12 -- 10k NTC -- GND
 *   pin 7: active-HIGH SSR DRIVER control; change SSR_PIN if necessary
 *   PJRC recommends at most 4 mA per GPIO. Use a suitable transistor/driver
 *   if the SSR input needs more; its 3-32 V label does not specify input current.
 *   Add a hardware pull-down at the driver input so reset/unpowered means OFF.
 *   Never connect the mains/load terminals to Teensy. Retain independent
 *   overtemperature protection, proper SSR heatsinking and a mains disconnect.
 *
 * COMMANDS (each followed by Enter, except ! which acts immediately):
 *   z 24.5        Calibrate at ACTUAL known tip temperature; start NEW CSV.
 *                 Tip/connector should be stable. Bare z does not assume 47 C.
 *   p 25          Manual 25% duty for 120 s, then heater OFF; log cooldown.
 *   p 25 60       Manual 25% duty for 60 s (1..600 s).
 *   p 0           Heater OFF; continue logging cooling.
 *   x             Same as p 0.
 *   h 100 180     PID target 100 C for 180 s, then heater OFF.
 *   t             Automatic low-temperature test: 80 -> 120 C -> cooling.
 *   f             Example Sn63/Pb37 reflow: 130 -> 150 -> 215 C -> cooling.
 *                 Disabled until HIGH_TEMP_VALIDATED is changed to true.
 *   k 2 0.03 8    Set Kp, Ki, Kd while heater is OFF; reset integral.
 *   c             Show all anticipation/startup settings and PID gains.
 *   c NAME VALUE  Change one setting while OFF; values reset on reboot.
 *     preview_s=25: center of future slope window, seconds (0..40)
 *     predict_s=15: target/temperature prediction horizon, seconds (0..30)
 *     ramp_ff=45: duty percentage points per C/s of future ramp (0..100)
 *     loss_ff=0.12: duty percentage points per C above run start (0..1)
 *     ahead_c=5: force zero when measured temp is this far ahead (0..10)
 *     window_s=10: future slope averaging window, seconds (1..30)
 *     rate_limit=3: prediction rate clamp in C/s (0.1..5)
 *     boost_pct=100, boost_s=15, boost_ahead_c=2: startup assist
 *       allowed ranges: 0..100%, 0..30 s, 0..5 C respectively.
 *   n             New CSV, same calibration, heater must be OFF.
 *   ?             Status and commands.
 *   s             Heater OFF, flush/close CSV; wait for CLOSED message.
 *   !             Immediate latched software STOP, even without newline.
 *   SD errors: stop logging, NOT heating. USB disconnect: no forced stop.
 *   Sensor jumps: up to 10 s of invalid readings tolerated with heater OFF;
 *                 two plausible readings are required before heating resumes.
 *   c spike_ignore_s 10   Set rejection timeout (0..10 s, heater OFF throughout).
 *   r             Clear fault only with fresh valid reading below 60 C.
 *                 Never restarts heating; recalibrate if sensor faulted.
 *
 * Start with p 25 60, x, inspect cooldown, then t from a cooled oven.
 * Changing a heating mode requires x first. A new run gets a new Run_ID;
 * Time_s is relative to the CSV, RunTime_s to that run. Use n between runs
 * if you prefer separate files. Calibration does NOT persist across reset.
 * Live Serial Monitor: time, filtered temperature, target, target-minus-temp
 * error, and commanded duty. Target/error show -- when not applicable.
 * Displayed time resets when a run starts; before the first run it uses CSV
 * elapsed time. Full measurement columns remain in the SD CSV; ? gives help
 * and detailed diagnostic readings. Command and fault messages remain visible.
 * t/f begin with 100% duty for up to 15 s, ending early if either
 * raw or filtered temperature reaches target + 2 C, then use preview + PID.
 * The profile clock is unchanged. Integral is frozen during this assist.
 * PWM_Command_pct logs the final command, including the startup minimum;
 * Profile feedforward previews ramp changes 25 s ahead. Feedback compares
 * the target 15 s ahead with temperature projected 15 s forward, so rising
 * temperature reduces power before a plateau. Current targets/clock do not
 * change. HOLD remains conventional PID. Defaults: Kp=6, Ki=0, Kd=4.
 * Error_C is the CURRENT target error; PID_P_pct uses the preview error
 * during profiles. New CSV fields record feedforward, prediction and settings.
 * Heating-only models fitted to three logs informed these provisional values;
 * simulation does not validate board temperature, sensor accuracy or cooling.
 *
 * IMPORTANT CALIBRATION:
 * Your supplied GAIN=112.06598985 and UV_PER_C=52 are preserved. 52 is not
 * the usual approximate K-type slope (~41 uV/C); it may be a J-type sensor
 * or an empirical correction. Verify sensor type and gain against a reference
 * thermometer across the intended range BEFORE enabling high-temperature
 * operation. Offset calibration alone cannot fix a wrong slope or sensor lag.
 * This retains your linear conversion and change-only cold-junction correction.
 * It is not full thermocouple linearization or reliable open-TC detection.
 *
 * CONTROL / LIMITATIONS:
 * A 1 s burst window commands the AC SSR: 25% = ~250 ms ON, 750 ms OFF.
 * No analogWrite / fast PWM / phase-angle control. Actual AC switching is
 * quantized by mains zero crossings. SSR duty columns describe commands,
 * NOT measured mains power or proof that an SSR has switched off.
 * Sensor/control/log rate is 2 Hz. PID derivative is on measured temperature,
 * with filtered rate and conditional integral anti-windup. Gains are un-tuned
 * starting values, not a guarantee of tracking on this oven.
 * The target timeline NEVER pauses to hide tracking error. Cooling forces
 * zero duty regardless of target error; there is no fan/door actuator.
 * Timer interrupt shuts the command off if fresh control data stops for 1.5 s,
 * provided interrupts still run. This is NOT an independent hardware watchdog.
 * Full MCU failure, shorted SSR, plausible-but-wrong temperature, detached tip
 * and some open-thermocouple faults require independent protection.
 * Noncritical SD errors and USB Serial disconnection do not abort an active
 * heating run. Thermocouple spikes are discarded for up to 10 s with heater OFF.
 * Two plausible readings are needed before PID resumes. An overtemperature
 * reading, bad ADC/NTC signal, or 10 s of unreliable data still trips a fault.
 * The profile clock continues throughout the interruption.
 *
 * SOURCES / DESIGN REFERENCES:
 * https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
 * https://www.ia.omron.com/data_pdf/guide/18/ssr_tg_e_9_2.pdf
 * https://www.ia.omron.com/product/cautions/18/safety_precautions.html
 * https://assets.omega.com/pdf/tables_and_graphs/thermocouple-type-k-celsius.pdf
 * https://www.kester.com/Portals/0/Documents/Knowledge%20Base/Standard_Profile.pdf
 * Assumes Sn63/Pb37 LEADED paste. The curve is an example, not a universal
 * leaded-solder specification: match your exact paste and component limits.
 * Host C++ checks are not a Teensyduino compile or a hardware validation.
 */

// Install appropriate extensions to run header files for arduino
// Or copy and paste code into Arduino IDE

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <IntervalTimer.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ---------------- Settings to review ----------------
const int SSR_PIN = 7;
const bool SSR_ACTIVE_HIGH = true;
const bool HIGH_TEMP_VALIDATED = true;  // true ONLY after checking thermometry
// Ignore non-safety USB Serial disconnects; physical emergency cutoff required.
const bool STOP_ON_USB_DISCONNECT = false;
const float HARD_LIMIT_C = HIGH_TEMP_VALIDATED ? 230.0f : 150.0f;
const float MAX_SETPOINT_C = HIGH_TEMP_VALIDATED ? 220.0f : 120.0f;
const float MAX_DUTY_PCT = 100.0f;        // duty does not limit ON-state current
const float LIQUIDUS_C = 183.0f;         // Sn63/Pb37; verify your exact alloy

const int TC_PIN = A10;
const int BIAS_PIN = A11;
const int THERMISTOR_PIN = A12;
const float ADC_REF = 3.3f;
const float ADC_MAX = 4095.0f;
const float DIVIDER = 0.5f;
const float GAIN = 112.06598985f;          // retained measured/selected gain
const float UV_PER_C = 52.0f;             // retained; verify K versus J/empirical
const float FIXED_RESISTOR_OHMS = 10000.0f;
const float NTC_RESISTANCE_25C = 10000.0f;
const float NTC_BETA_K = 3380.0f;
const float NTC_TEMPERATURE_25C_K = 298.15f;
const uint32_t SAMPLE_MS = 500;
const uint32_t FLUSH_MS = 1000;
const float TEMP_FILTER_TAU_S = 0.5f;
const float RATE_FILTER_TAU_S = 2.0f;
const float MAX_SENSOR_SLEW_C_PER_S = 20.0f; // abrupt jump = suspicious reading
// Only suspicious, in-range temperature jumps get this grace period.
// A real overtemperature, invalid ADC/NTC, or stalled control still stops heat.
float SPIKE_IGNORE_S = 10.0f; // maximum without trusted temperature, seconds
// Conservative estimate of how quickly the unpowered oven can coast.
// Large persistent offsets are never accepted as valid merely due to waiting.
float SPIKE_RECOVERY_MAX_RATE_CPS = 1.5f;
const float NO_RISE_FULL_POWER_S = 30.0f;    // heuristic, not open-TC detection
const uint32_t MAX_HEAT_RUN_MS = 600000;

// PID units: Kp [%/C], Ki [%/(C*s)], Kd [%*s/C]. Tune from measured runs.
float kp = 6.0f, ki = 0.0f, kd = 4.0f;

// Short startup assist for t/f only. Profile time continues normally.
// Applied as a duty floor, bounded by MAX_DUTY_PCT, not added to PID output.
float STARTUP_MIN_DUTY_PCT = 100.0f;
float STARTUP_ASSIST_S = 15.0f;
float STARTUP_AHEAD_C = 2.0f; // end early if raw OR filtered is ahead
bool startupAssistActive = true;

// Provisional profile feedforward from LOG0062/63/66 heating data.
// Preview upcoming slopes and predict temperature to reduce power early.
float PROFILE_PREVIEW_S = 25.0f;
float PROFILE_PREDICT_S = 15.0f;
float PROFILE_SLOPE_WINDOW_S = 10.0f;
float RAMP_FF_PCT_PER_CPS = 45.0f;
float LOSS_FF_PCT_PER_C = 0.12f;
float PROFILE_MAX_AHEAD_C = 5.0f;
float PREDICT_MAX_RATE_CPS = 3.0f;
struct ControlSetting {
  const char *name;
  float *value;
  float low, high;
};
ControlSetting controlSettings[] = {
  {"preview_s", &PROFILE_PREVIEW_S, 0, 40},
  {"predict_s", &PROFILE_PREDICT_S, 0, 30},
  {"ramp_ff", &RAMP_FF_PCT_PER_CPS, 0, 100},
  {"loss_ff", &LOSS_FF_PCT_PER_C, 0, 1},
  {"ahead_c", &PROFILE_MAX_AHEAD_C, 0, 10},
  {"window_s", &PROFILE_SLOPE_WINDOW_S, 1, 30},
  {"rate_limit", &PREDICT_MAX_RATE_CPS, 0.1f, 5},
  {"boost_pct", &STARTUP_MIN_DUTY_PCT, 0, 100},
  {"boost_s", &STARTUP_ASSIST_S, 0, 30},
  {"boost_ahead_c", &STARTUP_AHEAD_C, 0, 5},
  {"spike_ignore_s", &SPIKE_IGNORE_S, 0, 10},
  {"spike_recover_rate", &SPIKE_RECOVERY_MAX_RATE_CPS, 0.5f, 3.0f}
};
float feedforwardPct = 0, controlTargetC = NAN, predictedC = NAN;
float previewRateCps = 0;

struct ProfilePoint { float seconds; float celsius; };
// The first temperature is replaced by measured temperature at run start.
const ProfilePoint TEST_PROFILE[] = {
  {0, 25}, {60, 80}, {120, 120}, {150, 120}, {240, 50}
};
const ProfilePoint REFLOW_PROFILE[] = {
  {0, 25}, {90, 130}, {150, 150}, {225, 215}, {235, 215}, {275, 100}, {335, 50}
};
// Sn63/Pb37 example: 60 s soak from 130 to 150 C, peak 215 C with a 10 s
// plateau, then heater OFF at 235 s. The ideal target spends about 58 s above
// 183 C including cooling. Passive cooling may take longer: use the measured
// AboveLiquidus_s CSV field to assess the real result, not the target alone.
const float TEST_HEAT_END_S = 150.0f;
const float REFLOW_HEAT_END_S = 235.0f;

enum Mode { IDLE, MANUAL, HOLD, TEST, REFLOW, COOL, FAULT };
Mode mode = IDLE;
uint8_t profileKind = 0; // 0 = manual/hold, 1 = test, 2 = reflow
uint32_t runId = 0, runStartMs = 0, heatDurationMs = 0;
float runStartC = NAN, targetC = NAN, manualDuty = 0;
float dutyCommand = 0, pTerm = 0, iTerm = 0, dTerm = 0;
float noRiseAnchorC = NAN, noRiseDoseS = 0;
float peakC = NAN, aboveLiquidusS = 0;
const char *faultReason = "NONE";
const char *warningReason = "NONE";       // logged separately from latched safety faults
bool spikeHoldActive = false;
uint32_t spikeHoldStartMs = 0;
uint8_t spikeRecoverySamples = 0;
float spikeRecoveryFirstC = NAN;
uint32_t lastAcceptedTempMs = 0;
bool sdClosePending = false;

bool calibrated = false, sampleValid = false, timerOK = false;
bool haveTemperature = false;
float calibrationC = NAN, zeroOffset = 0, cjAtCalibration = NAN;
float outputVoltage = NAN, biasVoltage = NAN, cjTemp = NAN;
float rawC = NAN, filteredC = NAN, rateCps = 0;
uint32_t lastSampleMs = 0, lastValidSampleMs = 0;
File logFile;
char logName[13] = "";
bool logging = false;
uint32_t logStartMs = 0, lastFlushMs = 0;

IntervalTimer ssrTimer;
// Integer-only shared ISR state. No SD, Serial or float arithmetic in ISR.
volatile uint32_t timerTicks = 0, freshTick = 0, onTicks = 0;
volatile uint32_t requestedPct = 0;
volatile bool heaterArmed = false, staleLatch = false, ssrIsOn = false;
uint32_t lastLoggedTicks = 0, lastLoggedOnTicks = 0;

float clampf(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

bool heatingMode() {
  return mode == MANUAL || mode == HOLD || mode == TEST || mode == REFLOW;
}

const char *modeName() {
  switch (mode) {
    case MANUAL: return "MANUAL";
    case HOLD: return "HOLD";
    case TEST: return "TEST";
    case REFLOW: return "REFLOW";
    case COOL: return "COOL";
    case FAULT: return "FAULT";
    default: return "IDLE";
  }
}

void ssrInterrupt() {
  static uint32_t phase = 0, windowPct = 0;
  ++timerTicks; // one tick = 10 ms, including while disarmed
  if (heaterArmed && (uint32_t)(timerTicks - freshTick) >= 150) {
    staleLatch = true;
  }
  bool enabled = heaterArmed && !staleLatch;
  if (!enabled) {
    phase = 0;
    windowPct = 0;
  } else if (phase == 0) {
    windowPct = requestedPct; // latch duty for one whole 1 s window
  }
  // A zero command cancels even an unfinished ON pulse immediately.
  bool on = enabled && requestedPct != 0 && phase < windowPct;
  digitalWriteFast(SSR_PIN, on == SSR_ACTIVE_HIGH ? HIGH : LOW);
  ssrIsOn = on;
  if (on) ++onTicks;
  if (enabled) phase = (phase + 1) % 100;
}

void heaterOff() {
  startupAssistActive = false;
  noInterrupts();
  requestedPct = 0;
  heaterArmed = false;
  ssrIsOn = false;
  digitalWriteFast(SSR_PIN, SSR_ACTIVE_HIGH ? LOW : HIGH);
  interrupts();
  dutyCommand = 0;
}

void setDuty(float pct) {
  pct = clampf(pct, 0, MAX_DUTY_PCT);
  noInterrupts();
  if (!staleLatch && timerOK) {
    freshTick = timerTicks; // called ONLY after a fresh, valid sensor sample
    requestedPct = (uint32_t)(pct + 0.5f);
    heaterArmed = true;    // supervise even a temporary PID output of zero
    if (requestedPct == 0) {
      ssrIsOn = false;
      digitalWriteFast(SSR_PIN, SSR_ACTIVE_HIGH ? LOW : HIGH);
    }
  }
  interrupts();
  dutyCommand = staleLatch ? 0 : (float)requestedPct;
}

void tripFault(const char *reason) {
  heaterOff(); // ALWAYS before printing or touching SD
  if (mode != FAULT) {
    faultReason = reason;
    spikeHoldActive = false;
    spikeRecoverySamples = 0;
    mode = FAULT;
    Serial.print("# FAULT: "); Serial.println(reason);
    Serial.println("# Heater command OFF. Fix cause; r below 60 C clears latch.");
  }
}

void resetPID() {
  pTerm = iTerm = dTerm = 0;
  feedforwardPct = previewRateCps = 0;
  controlTargetC = predictedC = NAN;
  noRiseAnchorC = filteredC;
  noRiseDoseS = 0;
}

bool closeLog() {
  if (!logging && !sdClosePending) return true;
  logging = false;
  sdClosePending = false;
  logFile.flush();
  bool ok = !logFile.getWriteError();
  logFile.close();
  Serial.print(ok ? "# CLOSED " : "# CLOSED with SD error: ");
  Serial.println(logName);
  return ok;
}

// SD/CSV failures are informational for the heater controller, not thermal faults.
// If the file system blocks the main loop for too long, the independent SSR timer
// will still remove the heater command via STALE_CONTROL.
void disableLoggingWithWarning(const char *reason) {
  logging = false;                    // prevent any further writes this run
  warningReason = reason;
  Serial.print("# WARNING: "); Serial.println(reason);
  Serial.println("# SD logging stopped, but reflow control can continue.");
  // Avoid a potentially slow SD close while actively driving the oven.
  // Defer resource cleanup until the heater is off.
  if (heatingMode()) sdClosePending = true;
  else logFile.close();
}

bool writeLog(const char *data, size_t length) {
  if (logFile.write((const uint8_t *)data, length) != length ||
      logFile.getWriteError()) {
    disableLoggingWithWarning("SD_WRITE");
    return false;
  }
  return true;
}

bool startLogging() {
  heaterOff(); // caller must already have stopped any heating mode
  if (!closeLog()) { Serial.println("# WARNING: SD_CLOSE; cannot start new log."); return false; }
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("# ERROR: SD unavailable. Heating requires a working CSV.");
    return false;
  }
  bool found = false;
  for (unsigned int i = 1; i <= 9999; ++i) {
    snprintf(logName, sizeof(logName), "LOG%04u.CSV", i);
    if (!SD.exists(logName)) { found = true; break; }
  }
  if (!found) { Serial.println("# ERROR: LOG0001..LOG9999 all used."); return false; }
  logFile = SD.open(logName, FILE_WRITE);
  if (!logFile) { Serial.println("# ERROR: Cannot create CSV."); return false; }
  logging = true;
  logStartMs = lastFlushMs = millis();
  noInterrupts();
  lastLoggedTicks = timerTicks;
  lastLoggedOnTicks = onTicks;
  interrupts();
  const char header[] =
    "Time_s,Run_ID,RunTime_s,Mode,Target_C,Temperature_C,Raw_temperature_C,"
    "Error_C,PWM_Command_pct,SSR_Commanded_Duty_pct,SSR_Pin,Rate_C_per_s,"
    "Peak_C,AboveLiquidus_s,Liquidus_C,Amplifier_V,Bias_V,ColdJunction_C,"
    "CJ_Change_C,PID_P_pct,PID_I_pct,PID_D_pct,Kp,Ki,Kd,Gain,uV_per_C,"
    "Calibration_C,Fault,Warning,Feedforward_pct,ControlTarget_C,Predicted_C,PreviewRate_C_per_s,"
    "Preview_s,Predict_s,RampFF_pct_per_Cps,LossFF_pct_per_C,AheadLimit_C,"
    "SlopeWindow_s,PredictRateLimit_Cps,Startup_pct,Startup_s,StartupAhead_C,"
    "SpikeIgnore_s,SpikeRecoveryRate_Cps\r\n";
  if (!writeLog(header, sizeof(header) - 1)) return false;
  logFile.flush();
  if (logFile.getWriteError()) { disableLoggingWithWarning("SD_FLUSH"); return false; }
  warningReason = "NONE";
  Serial.print("# Saving "); Serial.println(logName);
  return true;
}

float readCounts(int pin) {
  float sum = 0;
  analogRead(pin); // discard first conversion after changing mux channel
  for (int i = 0; i < 64; ++i) sum += analogRead(pin);
  return sum / 64.0f;
}

bool readSensors() {
  float a = readCounts(TC_PIN), b = readCounts(BIAS_PIN);
  float n = readCounts(THERMISTOR_PIN);
  outputVoltage = a * ADC_REF / ADC_MAX / DIVIDER;
  biasVoltage = b * ADC_REF / ADC_MAX / DIVIDER;
  cjTemp = NAN;
  bool railsOK = a > 4 && a < ADC_MAX - 4 && b > 4 && b < ADC_MAX - 4;
  if (n > 4 && n < ADC_MAX - 4) {
    float r = FIXED_RESISTOR_OHMS * n / (ADC_MAX - n);
    float invK = 1.0f / NTC_TEMPERATURE_25C_K +
                 logf(r / NTC_RESISTANCE_25C) / NTC_BETA_K;
    if (invK > 0) cjTemp = 1.0f / invK - 273.15f;
  }
  return railsOK && isfinite(cjTemp) && cjTemp >= -20 && cjTemp <= 85;
}

float profileTarget(float seconds) {
  const ProfilePoint *points = profileKind == 2 ? REFLOW_PROFILE : TEST_PROFILE;
  size_t count = profileKind == 2 ? sizeof(REFLOW_PROFILE) / sizeof(ProfilePoint)
                                : sizeof(TEST_PROFILE) / sizeof(ProfilePoint);
  for (size_t i = 1; i < count; ++i) {
    if (seconds <= points[i].seconds) {
      float t0 = points[i - 1].seconds;
      float y0 = i == 1 ? runStartC : points[i - 1].celsius;
      float fraction = clampf((seconds - t0) / (points[i].seconds - t0), 0, 1);
      return y0 + fraction * (points[i].celsius - y0);
    }
  }
  return points[count - 1].celsius;
}

float pidDuty(float dt) {
  float error = targetC - filteredC;
  bool profileControl = mode == TEST || mode == REFLOW;
  feedforwardPct = previewRateCps = 0;
  controlTargetC = targetC;
  predictedC = filteredC;
  bool ahead = false;
  if (profileControl) {
    float elapsed = (uint32_t)(millis() - runStartMs) / 1000.0f;
    float endS = heatDurationMs / 1000.0f;
    // Clamp preview to heating end: never look into the cooling reference.
    float a = clampf(elapsed + PROFILE_PREVIEW_S - PROFILE_SLOPE_WINDOW_S / 2, 0, endS);
    float b = fminf(elapsed + PROFILE_PREVIEW_S + PROFILE_SLOPE_WINDOW_S / 2, endS);
    previewRateCps = fmaxf(0, (profileTarget(b) - profileTarget(a)) /
                            PROFILE_SLOPE_WINDOW_S);
    feedforwardPct = clampf(RAMP_FF_PCT_PER_CPS * previewRateCps +
        LOSS_FF_PCT_PER_C * fmaxf(0, targetC - runStartC), 0, MAX_DUTY_PCT);
    controlTargetC = profileTarget(fminf(elapsed + PROFILE_PREDICT_S, endS));
    predictedC = filteredC + clampf(rateCps, 0, PREDICT_MAX_RATE_CPS) * PROFILE_PREDICT_S;
    ahead = rawC >= targetC + PROFILE_MAX_AHEAD_C ||
            filteredC >= targetC + PROFILE_MAX_AHEAD_C;
  }
  pTerm = kp * (controlTargetC - predictedC);
  dTerm = -kd * rateCps; // derivative on measurement avoids setpoint kick
  // Integral still uses current target error, not the preview error.
  float trialI = clampf(iTerm + ki * error * dt, 0, MAX_DUTY_PCT);
  float trialOutput = feedforwardPct + pTerm + trialI + dTerm;
  // Integrate inside output limits, or only in a direction leaving saturation.
  // Freeze integral during startup assist so its extra power cannot wind it up.
  if (!startupAssistActive && !ahead && ((trialOutput >= 0 && trialOutput <= MAX_DUTY_PCT) ||
      (trialOutput > MAX_DUTY_PCT && error < 0) ||
      (trialOutput < 0 && error > 0))) iTerm = trialI;
  if (ahead) return 0;
  return clampf(feedforwardPct + pTerm + iTerm + dTerm, 0, MAX_DUTY_PCT);
}

void startCooling() {
  heaterOff();
  if (mode != FAULT) {
    mode = COOL;
    if (!profileKind) targetC = NAN;
    resetPID();
  }
}

void controlStep(uint32_t now, float dt) {
  if (mode == FAULT || mode == IDLE) { heaterOff(); return; }
  float elapsed = (uint32_t)(now - runStartMs) / 1000.0f;
  if (profileKind) targetC = profileTarget(elapsed);
  if (mode == COOL) {
    heaterOff();
    if (filteredC <= 50 && rawC <= 50) {
      mode = IDLE;
      targetC = NAN;
      Serial.println("# Below 50 C; run ended. CSV remains open.");
    }
    return;
  }
  if ((uint32_t)(now - runStartMs) >= MAX_HEAT_RUN_MS) {
    tripFault("MAX_RUN_TIME"); return;
  }
  if ((uint32_t)(now - runStartMs) >= heatDurationMs) {
    startCooling();
    Serial.println("# Heating finished. Heater OFF; logging cooling.");
    return;
  }
  // Latch assist OFF for the rest of this run once either condition is met.
  if (startupAssistActive &&
      (elapsed >= STARTUP_ASSIST_S ||
       rawC >= targetC + STARTUP_AHEAD_C ||
       filteredC >= targetC + STARTUP_AHEAD_C)) {
    startupAssistActive = false;
  }
  float power = mode == MANUAL ? manualDuty : pidDuty(dt);
  if (startupAssistActive) power = fmaxf(power, STARTUP_MIN_DUTY_PCT);
  // The configurable ahead limit also overrides the startup duty floor.
  if ((mode == TEST || mode == REFLOW) &&
      (rawC >= targetC + PROFILE_MAX_AHEAD_C ||
       filteredC >= targetC + PROFILE_MAX_AHEAD_C)) {
    power = 0;
    startupAssistActive = false;
  }
  bool expectRise = power >= 20 &&
                    (mode == MANUAL || targetC - filteredC > 10);
  if (!expectRise || filteredC >= noRiseAnchorC + 2) {
    noRiseAnchorC = filteredC;
    noRiseDoseS = 0;
  } else {
    noRiseDoseS += power * 0.01f * dt;
    if (noRiseDoseS >= NO_RISE_FULL_POWER_S) {
      tripFault("NO_TEMPERATURE_RISE"); return;
    }
  }
  setDuty(power);
}

float timeFractionAbove(float previous, float current) {
  if (previous >= LIQUIDUS_C && current >= LIQUIDUS_C) return 1;
  if (previous < LIQUIDUS_C && current < LIQUIDUS_C) return 0;
  if (current > previous) return (current - LIQUIDUS_C) / (current - previous);
  return (previous - LIQUIDUS_C) / (previous - current);
}

void printSerialStatus(uint32_t now) {
  if (!Serial) return;

  char temperatureText[20], targetText[20], errorText[20];
  if (spikeHoldActive && isfinite(filteredC))
    snprintf(temperatureText, sizeof(temperatureText), "HOLD %.1f", (double)filteredC);
  else if (isfinite(filteredC))
    snprintf(temperatureText, sizeof(temperatureText), "%.1f", (double)filteredC);
  else
    strcpy(temperatureText, "INVALID");

  if (isfinite(targetC))
    snprintf(targetText, sizeof(targetText), "%.1f", (double)targetC);
  else
    strcpy(targetText, "--");

  if (!spikeHoldActive && isfinite(targetC) && isfinite(filteredC))
    snprintf(errorText, sizeof(errorText), "%+.1f", (double)(targetC - filteredC));
  else
    strcpy(errorText, "--");

  float seconds = (uint32_t)(now - (runId ? runStartMs : logStartMs)) / 1000.0f;
  char status[180];
  int length = snprintf(status, sizeof(status),
    "Time: %.1fs | Temp: %s C | Target: %s C | Error: %s C | Duty: %.0f%%\r\n",
    (double)seconds, temperatureText, targetText, errorText, (double)dutyCommand);

  // Never wait for the host while controlling the heater.
  if (length > 0 && (size_t)length < sizeof(status) &&
      Serial.availableForWrite() >= length)
    Serial.write((const uint8_t *)status, (size_t)length);
}

void emitRow(uint32_t now) {
  uint32_t ticks, highTicks;
  bool pinOn;
  noInterrupts();
  ticks = timerTicks; highTicks = onTicks; pinOn = ssrIsOn;
  interrupts();
  uint32_t deltaTicks = ticks - lastLoggedTicks;
  float pinDuty = deltaTicks ? 100.0f * (highTicks - lastLoggedOnTicks) / deltaTicks : 0;
  lastLoggedTicks = ticks; lastLoggedOnTicks = highTicks;
  float elapsed = runId ? (uint32_t)(now - runStartMs) / 1000.0f : 0;
  char row[640];
  int n = snprintf(row, sizeof(row),
    "%.3f,%lu,%.3f,%s,%.3f,%.3f,%.3f,%.3f,%.1f,%.1f,%u,%.3f,%.3f,"
    "%.3f,%.1f,%.6f,%.6f,%.3f,%.3f,%.3f,%.3f,%.3f,%.4f,%.5f,%.4f,"
    "%.8f,%.3f,%.3f,%s,%s,%.3f,%.3f,%.3f,%.3f,"
    "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
    (double)((uint32_t)(now - logStartMs) / 1000.0f), (unsigned long)runId,
    (double)elapsed, modeName(), (double)targetC, (double)filteredC,
    (double)rawC, (double)(targetC - filteredC), (double)dutyCommand,
    (double)pinDuty, pinOn ? 1U : 0U, (double)rateCps, (double)peakC,
    (double)aboveLiquidusS, (double)LIQUIDUS_C, (double)outputVoltage,
    (double)biasVoltage, (double)cjTemp, (double)(cjTemp - cjAtCalibration),
    (double)pTerm, (double)iTerm, (double)dTerm, (double)kp, (double)ki,
    (double)kd, (double)GAIN, (double)UV_PER_C, (double)calibrationC,
    faultReason, warningReason, (double)feedforwardPct, (double)controlTargetC, (double)predictedC,
    (double)previewRateCps, (double)PROFILE_PREVIEW_S, (double)PROFILE_PREDICT_S,
    (double)RAMP_FF_PCT_PER_CPS, (double)LOSS_FF_PCT_PER_C,
    (double)PROFILE_MAX_AHEAD_C, (double)PROFILE_SLOPE_WINDOW_S,
    (double)PREDICT_MAX_RATE_CPS, (double)STARTUP_MIN_DUTY_PCT,
    (double)STARTUP_ASSIST_S, (double)STARTUP_AHEAD_C,
    (double)SPIKE_IGNORE_S, (double)SPIKE_RECOVERY_MAX_RATE_CPS);
  if (n <= 0 || (size_t)n >= sizeof(row)) {
    if (logging) disableLoggingWithWarning("CSV_FORMAT");
    printSerialStatus(now);
    return;
  }
  printSerialStatus(now);
  if (logging && writeLog(row, (size_t)n) &&
      (uint32_t)(now - lastFlushMs) >= FLUSH_MS) {
    logFile.flush();
    lastFlushMs = millis();
    if (logFile.getWriteError()) disableLoggingWithWarning("SD_FLUSH");
  }
}

void sampleStep() {
  sampleValid = readSensors();
  uint32_t now = millis();
  float dt = lastSampleMs ? (uint32_t)(now - lastSampleMs) / 1000.0f
                         : SAMPLE_MS / 1000.0f;
  dt = fmaxf(dt, 0.001f);
  lastSampleMs = now;
  if (!sampleValid) {
    rawC = filteredC = NAN;
    haveTemperature = false;
    if (calibrated || heatingMode()) {
      tripFault("ADC_OR_NTC_INVALID");
      calibrated = false;
    }
    if (logging) emitRow(now);
    return;
  }
  if (!calibrated) return;
  float signal = outputVoltage - biasVoltage;
  float nextRaw = calibrationC +
    (signal - zeroOffset) * 1000000.0f / (GAIN * UV_PER_C) +
    (cjTemp - cjAtCalibration);
  if (!isfinite(nextRaw) || nextRaw < -20) {
    tripFault("TEMPERATURE_INVALID");
    calibrated = false; haveTemperature = false;
    rawC = filteredC = NAN;
    if (logging) emitRow(now);
    return;
  }

  // Always enforce the raw hardware temperature limit BEFORE spike rejection.
  // A high reading is NEVER dismissed as a harmless glitch.
  if (nextRaw >= HARD_LIMIT_C) {
    rawC = nextRaw;
    tripFault("OVERTEMPERATURE");
    if (logging) emitRow(now);
    return;
  }

  // Judge against the most recent accepted sensor sample. The allowed
  // temperature difference grows with elapsed time while heating is OFF.
  float acceptedDt = lastAcceptedTempMs
      ? fmaxf((uint32_t)(now - lastAcceptedTempMs) / 1000.0f, dt)
      : dt;
  bool jump = haveTemperature &&
      fabsf(nextRaw - rawC) > MAX_SENSOR_SLEW_C_PER_S * acceptedDt;
  if (spikeHoldActive && haveTemperature &&
      fabsf(nextRaw - rawC) > SPIKE_RECOVERY_MAX_RATE_CPS * acceptedDt) {
    jump = true;  // reject a sustained but impossible sensor offset
  }

  // A fresh-looking reading following a spike is not enough by itself;
  // require a second consecutive plausible reading before power resumes.
  if (spikeHoldActive && spikeRecoverySamples == 1 &&
      fabsf(nextRaw - spikeRecoveryFirstC) >
          MAX_SENSOR_SLEW_C_PER_S * fmaxf(dt, SAMPLE_MS / 1000.0f)) {
    jump = true;
  }

  if (jump && !spikeHoldActive) {
    spikeHoldActive = true;
    spikeHoldStartMs = now;
    spikeRecoverySamples = 0;
    warningReason = "TEMP_SPIKE_IGNORED";
    Serial.println("# WARNING: implausible temperature; heating paused for up to 10 s.");
  }

  if (spikeHoldActive) {
    // Any uncertain reading leaves the SSR OFF immediately. The timer remains
    // disarmed, so a stale reading cannot keep the heater energized.
    heaterOff();
    if ((uint32_t)(now - spikeHoldStartMs) >=
        (uint32_t)(SPIKE_IGNORE_S * 1000.0f)) {
      tripFault("TEMPERATURE_JUMP_TIMEOUT");
      calibrated = false;
      if (logging) emitRow(now);
      return;
    }
    if (jump) {
      spikeRecoverySamples = 0;
      if (logging) emitRow(now); // keep last good temperature, log warning
      return;
    }
    if (spikeRecoverySamples == 0) {
      spikeRecoveryFirstC = nextRaw;
      spikeRecoverySamples = 1;
      if (logging) emitRow(now);
      return;
    }
    // Two plausible readings received; resume the existing profile.
    spikeHoldActive = false;
    spikeRecoverySamples = 0;
    if (strcmp(warningReason, "TEMP_SPIKE_IGNORED") == 0)
      warningReason = "NONE";
    Serial.println("# Sensor readings recovered; reflow continues.");
  }
  lastValidSampleMs = now;
  lastAcceptedTempMs = now;
  float previousFiltered = filteredC;
  bool hadTemperature = haveTemperature;
  rawC = nextRaw;
  if (!hadTemperature) { filteredC = rawC; rateCps = 0; }
  else {
    filteredC += acceptedDt / (TEMP_FILTER_TAU_S + acceptedDt) * (rawC - filteredC);
    float instantRate = (filteredC - previousFiltered) / acceptedDt;
    rateCps += acceptedDt / (RATE_FILTER_TAU_S + acceptedDt) * (instantRate - rateCps);
  }
  haveTemperature = true;
  if (runId) {
    peakC = isfinite(peakC) ? fmaxf(peakC, rawC) : rawC;
    // Do not guess time-above-liquidus through an unmeasured sensor gap.
    // At most one fresh sampling interval counts after a recovered dropout.
    if (hadTemperature && mode != IDLE)
      aboveLiquidusS += fminf(acceptedDt, dt) *
                         timeFractionAbove(previousFiltered, filteredC);
  }
  if (filteredC >= HARD_LIMIT_C) tripFault("OVERTEMPERATURE");
  if (staleLatch) tripFault("STALE_CONTROL");
  // Keep PID integration bounded to the current sample. Do not integrate
  // the entire gap while the sensor was unavailable and power was off.
  if (calibrated && mode != FAULT) controlStep(now, dt);
  if (logging) emitRow(now);
}

bool startRun(Mode newMode, float seconds); // explicit prototype for Arduino
bool startRun(Mode newMode, float seconds) {
  if (mode == FAULT) { Serial.println("# ERROR: fault latched; inspect then r."); return false; }
  if (heatingMode()) { Serial.println("# ERROR: send x before starting/changing run."); return false; }
  if (!timerOK || !calibrated || !sampleValid || !haveTemperature ||
      (uint32_t)(millis() - lastValidSampleMs) > 1000 || rawC >= HARD_LIMIT_C) {
    Serial.println("# ERROR: need fresh calibrated sensor and working timer."); return false;
  }
  if (STOP_ON_USB_DISCONNECT && !Serial) return false;
  if ((newMode == TEST || newMode == REFLOW) && (rawC > 50 || filteredC > 50)) {
    Serial.println("# ERROR: cool below 50 C before a profile run."); return false;
  }
  if (!logging && !startLogging()) return false;
  // Opening SD may take time; force a fresh reading before any duty is applied.
  if ((uint32_t)(millis() - lastSampleMs) >= SAMPLE_MS) sampleStep();
  if (!calibrated || !sampleValid || mode == FAULT) return false;
  ++runId;
  runStartMs = millis();
  runStartC = filteredC;
  heatDurationMs = (uint32_t)(seconds * 1000.0f);
  profileKind = newMode == REFLOW ? 2 : (newMode == TEST ? 1 : 0);
  peakC = rawC; aboveLiquidusS = 0;
  resetPID();
  mode = newMode;
  startupAssistActive = (newMode == TEST || newMode == REFLOW);
  if (newMode == MANUAL) targetC = NAN;
  if (profileKind) targetC = profileTarget(0);
  Serial.print("# Starting "); Serial.println(modeName());
  return true; // heater remains OFF until the next fresh sample/control step
}

void printHelp() {
  Serial.println("# z TEMP = calibrate/new CSV; p PCT [SECONDS=120] = manual burst");
  Serial.println("# h TEMP [SECONDS=180] = PID hold; t = 120 C test; f = example reflow");
  Serial.println("# c = show control settings; c NAME VALUE = change while heater OFF (RAM only)");
  Serial.println("# c spike_ignore_s 10 = tolerate up to 10 s of spikes, HEATER OFF during them");
  Serial.println("# c spike_recover_rate 1.5 = max plausible unpowered temperature drift C/s");
  Serial.println("# x or p 0 = heat off/log cooling; s = heat off/close CSV; ! = latched STOP");
  Serial.println("# k KP KI KD = tune while off; n = new CSV; r = clear fault; ? = help/status");
  Serial.print("# Mode="); Serial.print(modeName());
  Serial.print(" T="); Serial.print(rawC, 2);
  Serial.print(" C Target="); Serial.print(targetC, 2);
  Serial.print(" Duty="); Serial.print(dutyCommand, 1);
  Serial.print("% Cutoff="); Serial.print(HARD_LIMIT_C, 0);
  Serial.print(" C Fault="); Serial.print(faultReason);
  Serial.print(" Warning="); Serial.println(warningReason);
  Serial.print("# Spike ignore timeout: ");
  Serial.print(SPIKE_IGNORE_S, 1); Serial.println(" s (heater OFF during rejection)");
  Serial.print("# Amplifier="); Serial.print(outputVoltage, 4);
  Serial.print(" V Bias="); Serial.print(biasVoltage, 4);
  Serial.print(" V CJ="); Serial.print(cjTemp, 2); Serial.println(" C");
}

void handleCommand(char *line) {
  char *words[5];
  size_t count = 0;
  char *token = strtok(line, " \t");
  while (token && count < 5) { words[count++] = token; token = strtok(NULL, " \t"); }
  if (!count) return;
  if (token || strlen(words[0]) != 1) { Serial.println("# ERROR: invalid command."); return; }
  char cmd = (char)tolower((unsigned char)words[0][0]);
  if (cmd == 'c') {
    if (count == 1) {
      for (const ControlSetting &setting : controlSettings) {
        Serial.print("# c "); Serial.print(setting.name); Serial.print(" ");
        Serial.print(*setting.value, 3); Serial.print(" [");
        Serial.print(setting.low, 2); Serial.print("..");
        Serial.print(setting.high, 2); Serial.println("]");
      }
      Serial.print("# k "); Serial.print(kp, 3); Serial.print(" ");
      Serial.print(ki, 5); Serial.print(" "); Serial.println(kd, 3);
      return;
    }
    if (count != 3 || heatingMode()) {
      Serial.println("# ERROR: heater OFF; use c NAME VALUE or c to list."); return;
    }
    char *end;
    float value = strtof(words[2], &end);
    if (end == words[2] || *end || !isfinite(value)) {
      Serial.println("# ERROR: expected finite setting value."); return;
    }
    for (ControlSetting &setting : controlSettings) {
      if (strcmp(words[1], setting.name) == 0) {
        if (value < setting.low || value > setting.high) {
          Serial.println("# ERROR: outside setting range; c lists limits."); return;
        }
        *setting.value = value; resetPID();
        Serial.print("# Updated "); Serial.print(setting.name); Serial.print("=");
        Serial.println(value, 3); return;
      }
    }
    Serial.println("# ERROR: unknown setting; c lists names."); return;
  }
  float args[4] = {0};
  for (size_t i = 1; i < count; ++i) {
    char *end;
    args[i - 1] = strtof(words[i], &end);
    if (end == words[i] || *end || !isfinite(args[i - 1])) {
      Serial.println("# ERROR: expected finite number."); return;
    }
  }
  if (cmd == '?' && count == 1) { printHelp(); return; }
  if (cmd == 's' && count == 1) {
    heaterOff();
    if (mode != FAULT) { mode = IDLE; targetC = NAN; profileKind = 0; resetPID(); }
    if (!closeLog()) tripFault("SD_CLOSE");
    return;
  }
  if (cmd == 'x' && count == 1) {
    startCooling();
    profileKind = 0; targetC = NAN;
    Serial.println("# Heater OFF; CSV remains open."); return;
  }
  if (cmd == 'r' && count == 1) {
    heaterOff();
    if (mode != FAULT) { Serial.println("# No fault to clear."); return; }
    // If calibration was lost, require a new explicit calibration below 60 C.
    // A plausible ADC signal alone cannot prove a cold tip.
    if (!sampleValid || (uint32_t)(millis() - lastValidSampleMs) > 1000 ||
        !isfinite(rawC) || rawC >= 60 || !calibrated) {
      Serial.println("# Reset blocked: need valid calibrated tip below 60 C.");
      Serial.println("# If sensor calibration was lost: cool, repair, then z KNOWN_TEMP.");
      return;
    }
    noInterrupts(); staleLatch = false; interrupts();
    mode = IDLE; profileKind = 0; targetC = NAN; faultReason = "NONE";
    resetPID(); Serial.println("# Fault cleared; heater stays OFF."); return;
  }
  if (cmd == 'z' && count == 2) {
    if (heatingMode() || args[0] < -10 || args[0] > 80) {
      Serial.println("# ERROR: stop heat first; known calibration must be -10..80 C."); return;
    }
    // Explicit calibration is also the recovery path for a repaired sensor.
    // It NEVER clears a fault or turns on heat; r is still required afterward.
    heaterOff();
    sampleValid = readSensors();
    if (!sampleValid) { Serial.println("# ERROR: ADC/NTC invalid; calibration refused."); return; }
    zeroOffset = outputVoltage - biasVoltage;
    cjAtCalibration = cjTemp; calibrationC = args[0];
    rawC = filteredC = calibrationC; rateCps = 0;
    calibrated = haveTemperature = true;
    lastSampleMs = lastValidSampleMs = lastAcceptedTempMs = millis();
    spikeHoldActive = false;
    spikeRecoverySamples = 0;
    warningReason = "NONE";
    runId = 0; profileKind = 0; targetC = NAN; peakC = NAN; aboveLiquidusS = 0;
    if (mode != FAULT) mode = IDLE;
    resetPID();
    if (startLogging()) Serial.println("# Calibrated at supplied temperature; heater OFF.");
    return;
  }
  if (cmd == 'n' && count == 1) {
    if (heatingMode() || mode == FAULT || !calibrated) {
      Serial.println("# ERROR: n requires calibrated sensor, no fault, heater OFF."); return;
    }
    startLogging(); return;
  }
  if (cmd == 'k' && count == 4) {
    if (heatingMode() || args[0] < 0 || args[0] > 100 || args[1] < 0 ||
        args[1] > 10 || args[2] < 0 || args[2] > 100) {
      Serial.println("# ERROR: heater OFF; Kp 0..100, Ki 0..10, Kd 0..100."); return;
    }
    kp = args[0]; ki = args[1]; kd = args[2]; resetPID();
    Serial.println("# PID gains updated (RAM only)."); return;
  }
  if (cmd == 'p' && (count == 2 || count == 3)) {
    float seconds = count == 3 ? args[1] : 120;
    if (args[0] < 0 || args[0] > MAX_DUTY_PCT || seconds < 1 || seconds > 600) {
      Serial.println("# ERROR: duty outside limit or duration outside 1..600 s."); return;
    }
    if (args[0] == 0) {
      startCooling(); profileKind = 0; targetC = NAN;
      Serial.println("# Heater OFF; CSV remains open."); return;
    }
    if (startRun(MANUAL, seconds)) manualDuty = args[0];
    return;
  }
  if (cmd == 'h' && (count == 2 || count == 3)) {
    float seconds = count == 3 ? args[1] : 180;
    if (args[0] < 25 || args[0] > MAX_SETPOINT_C || seconds < 1 || seconds > 600) {
      Serial.println("# ERROR: target outside allowed range or duration outside 1..600 s."); return;
    }
    if (startRun(HOLD, seconds)) targetC = args[0];
    return;
  }
  if (cmd == 't' && count == 1) { startRun(TEST, TEST_HEAT_END_S); return; }
  if (cmd == 'f' && count == 1) {
    if (!HIGH_TEMP_VALIDATED) {
      Serial.println("# Reflow disabled. Verify thermometry/paste profile, then set");
      Serial.println("# HIGH_TEMP_VALIDATED=true in source and re-upload."); return;
    }
    startRun(REFLOW, REFLOW_HEAT_END_S); return;
  }
  Serial.println("# ERROR: unknown command or arguments; ? for help.");
}

void readCommands() {
  static char line[80];
  static size_t used = 0;
  static bool discard = false;
  // Bound work per loop so a continuous stream cannot starve sampling.
  for (unsigned int i = 0; i < 96 && Serial.available(); ++i) {
    char c = (char)Serial.read();
    if (c == '!') {
      tripFault("USER_STOP"); used = 0; discard = true;
      continue;
    }
    if (c == '\r' || c == '\n') {
      if (!discard && used) { line[used] = 0; handleCommand(line); }
      used = 0; discard = false; continue;
    }
    if (discard) continue;
    if (used + 1 >= sizeof(line)) {
      used = 0; discard = true;
      Serial.println("# ERROR: command too long; discarded.");
    } else if (c >= 32 || c == '\t') line[used++] = c;
  }
}

void setup() {
  digitalWrite(SSR_PIN, SSR_ACTIVE_HIGH ? LOW : HIGH);
  pinMode(SSR_PIN, OUTPUT);
  heaterOff();
  timerOK = ssrTimer.begin(ssrInterrupt, 10000); // 10 ms interrupt
  ssrTimer.priority(64);
  Serial.begin(115200);
  analogReadResolution(12);
  analogReadAveraging(16);
  while (!Serial && millis() < 3000) {}
  if (!timerOK) tripFault("TIMER_INIT");
  Serial.println("# Oven SSR curve tester ready; heater OFF.");
  Serial.println("# UV_PER_C=52 retained: verify sensor/calibration before high temperature.");
  sampleStep();
  printHelp();
}

void loop() {
  if (staleLatch && mode != FAULT) tripFault("STALE_CONTROL");
  if (heatingMode() && STOP_ON_USB_DISCONNECT && !Serial) tripFault("USB_DISCONNECTED");
  readCommands();
  if ((uint32_t)(millis() - lastSampleMs) >= SAMPLE_MS) sampleStep();
  if (sdClosePending && !heatingMode()) closeLog();
}
