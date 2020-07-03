#include <SPI.h>
#define CS_PIN PA8
SPIClass SPI_2(2);
byte data;
float radian = 0.0174532925;
float max_bits = 4096;                                                //Change if you use a different setup
float max_scan_angle_degree = 25;                                     //Change if you use a different setup
float half_axis_lenght = 200;                                         //Change this value to get the right dimensions for cutting and engraving
float steps_per_unit = 100;                                           //Same as in your controller config
float max_scan_angle_radian = max_scan_angle_degree * radian;
float height = half_axis_lenght / tan(max_scan_angle_radian / 2);
float bit_per_angle = max_bits / max_scan_angle_radian;
float origin = 0 - half_axis_lenght;
volatile long steps = 0;

void setOutput(unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x10;
  digitalWrite(CS_PIN, LOW);
  SPI_2.transfer(highByte);
  SPI_2.transfer(lowByte);
  digitalWrite(CS_PIN, HIGH);
}

void pulse() {
  if (digitalRead(PB7) == LOW) {
    steps = steps + 1;
    if (steps > (half_axis_lenght * steps_per_unit)) {
      steps = half_axis_lenght * steps_per_unit;
    }
  }
  if (digitalRead(PB7) == HIGH) {
    steps = steps - 1;
    if (steps < origin * steps_per_unit)
    {
      steps = origin * steps_per_unit ;
    }
  }
}


void setup() {
  SPI_2.begin();
  SPI_2.setBitOrder(MSBFIRST);
  SPI_2.setDataMode(SPI_MODE0);
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);
  pinMode(CS_PIN, OUTPUT);
  Serial.begin(115200);
  pinMode(PB7, INPUT);
  pinMode(PB8, INPUT);
  pinMode(PB9, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PB8), pulse, RISING);
}

void loop()
{
  float coordinate = steps / steps_per_unit;
  float scan_angle = atan(coordinate / height);
  float angle_to_dac = scan_angle * bit_per_angle;
  unsigned int dac_value = map(angle_to_dac, -2048, 2048, 0, 4095);
  setOutput(dac_value);
  Serial.println(half_axis_lenght);
  delay(10);

  if (dac_value == 0) {
    digitalWrite(PB9, HIGH);
  }
  else {
    digitalWrite(PB9, LOW);
  }
}
