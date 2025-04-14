#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET -1
#define SDA_PIN 8
#define SCL_PIN 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/////////////Change your data below/////////////
////////////////////////////////////////////////
const int MQ_PIN = 0;     //Analog pin connected to sensor
float RL_VALUE = 16.667;  //Sensor RL Resistance in Kilo ohms - 20k + 100k poti in parallel as voltage devider. 66/34k adjusted for 3.3v
float R0 = 0.070;        //R0 Resistance in kilo ohms (use other code to get it)
// Variables we need in the code
const int READ_SAMPLE_INTERVAL = 100;  //Time between samples
const int READ_SAMPLE_TIMES = 5;       //Amount of samples
int Adc_Res = 4096;                    //Analog Digital Converter Resolution - 10 bit=1024, 12 bit=4096, 16 bit=65536 etc.
// Add next points values according to your dataseheet Rs/R0 graph
const float X0 = 50;
const float Y0 = 0.18;
const float X1 = 500;
const float Y1 = 0.012;
float BAC1;
float BAC2;
float BAC3;
// Calculate the curve points {X, Y}
const float point0[] = { log10(X0), log10(Y0) };
const float point1[] = { log10(X1), log10(Y1) };
// Calculate the values of the graph
const float scope = (point1[1] - point0[1]) / (point1[0] - point0[0]);
const float coord = point0[1] - point0[0] * scope;

// Obtain the Voltage at x samples
float readMQ(int mq_pin) {
  float rs = 0;
  for (int i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += getMQResistance(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  return rs / READ_SAMPLE_TIMES;
}

// Obtain the resistance
float getMQResistance(int raw_adc) {
  return (((float)RL_VALUE / 1000.0 * (Adc_Res - raw_adc) / raw_adc));  //adc resoluyion 4096 = 12 bit
}

// Obtain the alcohol concentration at 10^(coord + scope * log (rs/r0)
float getConcentration(float rs_ro_ratio) {
  return pow(10, coord + scope * log(rs_ro_ratio));
}

void setup() {
  //pinMode(MQ_PIN, INPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(100);
}

void loop() {
  float rs_med = readMQ(MQ_PIN);  // Get mesn RS resistance value
  float concentration = getConcentration(rs_med / R0);  // Get concentration

  // Display the value
  float BAC = map(concentration, 20, 500, 7, 184);  //20 to 500 scale is from the MQ-3 datasheet graph
  BAC1 = BAC / 100; //100 shows blood alcoholconcentration in permille on display
  //Serial Print
  Serial.print("BAC: ");
  Serial.println(BAC1, 3);
  Serial.print("PPM: ");
  Serial.println(concentration, 0);
  //Dislaying Values
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ALCOHOLTEST");
  display.setTextSize(2);
  display.print("PPM:");
  display.println(concentration, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("BAC:");
  display.println(BAC1, 2);
  BAC2 = BAC1 *100;
  BAC3 = map(BAC2, 0, 100, 0, 127);
  if (BAC3 > 127) {
  BAC3 = 127;
  }
  display.fillRect(36, 46, 2, 10, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(42, 46);
  display.print("0.3");
  display.fillRect(86, 46, 2, 10, WHITE);
  display.setTextColor(WHITE);
  display.setCursor(92, 46);
  display.print("0.7");
  display.fillRect(0, 55, BAC3, 9, WHITE);
  display.display();
}
