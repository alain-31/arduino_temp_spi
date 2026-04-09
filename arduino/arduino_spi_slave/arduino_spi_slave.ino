#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>

const uint8_t MPU_ADDR = 0x68;

static float gx_f_dps = 0;
static float temp_f_c = 0;

int16_t gx_offset = 0;

struct __attribute__((packed)) SensorData {
  int16_t ax;
  int16_t ay;
  int16_t az;
  float gx;
  float gy;
  float gz;
  float temp;
};

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

// SPI test packet
volatile SensorData sensorData;
volatile uint8_t txIndex = 0;

// For SS tracking
bool previousSS = HIGH;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // LCD init
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Init MPU6050...");

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);   // PWR_MGMT_1
  Wire.write(0x00);   // wake up
  Wire.endTransmission(true);

  // centering gx
  // compute the offset from 200 samples
  long sum = 0;

  for (int i = 0; i < 200; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43);   // GYRO_XOUT_H
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, (uint8_t)2, (uint8_t)true);

    if (Wire.available() >= 2) {
      int16_t gx = (Wire.read() << 8) | Wire.read();
      sum += gx;
    }

    delay(5);
  }

  gx_offset = sum / 200;

  // SPI slave pins
  pinMode(MISO, OUTPUT);  // Arduino sends data on MISO
  pinMode(SS, INPUT_PULLUP);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);

  // Enable SPI in slave mode + interrupt
  SPCR |= _BV(SPE);   // SPI enable
  SPCR |= _BV(SPIE);  // SPI interrupt enable

  // preload first byte
  SPDR = ((volatile uint8_t *)&sensorData)[0];
  txIndex = 1;

  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MPU+SPI ready");
  Serial.println("MPU6050 + SPI slave ready");
  delay(1000);
  lcd.clear();
}

ISR(SPI_STC_vect) {
  volatile uint8_t received = SPDR;  // byte received from master
  (void)received;                    // unused for now

  // Send next byte
  if (txIndex < sizeof(SensorData)) {
    SPDR = ((volatile uint8_t *)&sensorData)[txIndex++];
  } else {
    SPDR = 0xEE;
  }
}

void loop() {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true);

  if (Wire.available() >= 14) {
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();

    int16_t temp_raw = (Wire.read() << 8) | Wire.read();

    int16_t gx = (Wire.read() << 8) | Wire.read();
    int16_t gy = (Wire.read() << 8) | Wire.read();
    int16_t gz = (Wire.read() << 8) | Wire.read();

    float temp_c = temp_raw / 340.0 + 36.53;

    // calibration, filtering, round for display
    float gx_dps = (gx - gx_offset) / 131.0;
    gx_f_dps = 0.9 * gx_f_dps + 0.1 * gx_dps;
    float gx_disp = roundf(gx_f_dps * 2.0f) / 2.0f;

    // conversion 
    float gy_dps = gy / 131.0;
    float gz_dps = gz / 131.0;

    //  filtering, round for display
    temp_f_c= 0.1 * temp_f_c + 0.9 * temp_c;
    float temp_disp = roundf(temp_f_c * 2.0f) / 2.0f;

    noInterrupts();
    sensorData.ax = ax;
    sensorData.ay = ay;
    sensorData.az = az;
    sensorData.gx = gx_f_dps;
    sensorData.gy = gy_dps;
    sensorData.gz = gz_dps;
    sensorData.temp = temp_disp;
    interrupts();

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp_disp, 1);
    lcd.print((char)223);
    lcd.print("C   ");
 }

  if (digitalRead(SS) == HIGH) {
    txIndex = 1;
    SPDR = ((volatile uint8_t *)&sensorData)[0];
  }

  delay(100);
}