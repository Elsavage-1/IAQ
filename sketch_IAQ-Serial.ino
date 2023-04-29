#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_PM25AQI.h>
#include <Adafruit_SCD30.h>


//Feather wing OLED
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

//SCD30 CO2, Temp, Humdity sensor
Adafruit_SCD30  scd30 = Adafruit_SCD30();

//PM25 AQI Sensor
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// OLED FeatherWing buttons only including 32u4, M0, M4, nrf52840, esp32-s2 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5


void setup() {
  Serial.begin(115200);
  //while (!Serial) delay(10);
  Serial.println("128x64 OLED FeatherWing test");
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default

  Serial.println("Serial Output Test");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Making sure PM25 boots
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  Serial.println("PM25 found!");


  //making sure SCD30 boots
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  Serial.println("SCD30 Found!");
  scd30.setMeasurementInterval(10);
  Serial.print("Measurement Interval: "); 
  Serial.print(scd30.getMeasurementInterval()); 
  Serial.println(" seconds");


  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  Serial.println("Button test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.print("Boooting.....");
  display.display(); // actually display all of the above
  delay(1000);
  display.clearDisplay();





}
int last_button_press = 0;

void loop() {
  if(!digitalRead(BUTTON_A)) last_button_press = 'A';
  if(!digitalRead(BUTTON_B)) last_button_press = 'B';
  if(!digitalRead(BUTTON_C)) last_button_press = 'C';

  display.clearDisplay();
  scd30.read();
  PM25_AQI_Data data;

  switch (last_button_press) {
    case 'A':
    Serial.println('Button A');
    Serial.print("Temperature: ");
    Serial.print(scd30.temperature);
    Serial.println(" degrees C");
    
    Serial.print("Relative Humidity: ");
    Serial.print(scd30.relative_humidity);
    Serial.println(" %");
    
    Serial.print("CO2: ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");
    Serial.println("");
      display_scd30_data();
      break;

    case 'B':
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Hello World :3");
      display.println("Notices your IAQ tool OwO");
      break;

    case 'C':
        Serial.println('Button C');
      display_aqi_data();
      break;

    default:
      display.setCursor(0, 0);
      display.println("Press a button");
      break;
    Serial.print("CO2: ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");
    Serial.println("");
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
 
  }

  display.display();

  delay(10);
  yield();
}

void display_scd30_data() {
  static float prev_temp = 0.0f;
  static float prev_humidity = 0.0f;
  static float prev_co2 = 0.0f;

  Serial.println("START");
  
  while(true) {
    if (scd30.dataReady()) {
      Serial.println("Data available!");
      if (!scd30.read()) {
        Serial.println("Error reading sensor data");
        return;
      }
      
      float new_temp = scd30.temperature;
      float new_humidity = scd30.relative_humidity;
      float new_co2 = scd30.CO2;

      // Check if new data is not 'nan' and update the previous values
      if(!isnan(new_temp) && new_temp != prev_temp) {
        prev_temp = new_temp;
      }
      
      if(!isnan(new_humidity) && new_humidity != prev_humidity) {
        prev_humidity = new_humidity;
      }
      
      if(!isnan(new_co2) && new_co2 != prev_co2) {
        prev_co2 = new_co2;
      }

      // Clear display and print updated values
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.println("SCD30 Data");

      display.setTextSize(1);
      display.setCursor(0, 25);
      display.print("Temperature: ");
      display.print(prev_temp);
      display.println(" C");

      display.print("Humidity: ");
      display.print(prev_humidity);
      display.println(" %");

      display.print("CO2: ");
      display.print(prev_co2, 1);
      display.println(" ppm");
      display.display();
      delay(100);
      
          // Check for button press 'B' or 'C' to exit while loop
    if (digitalRead(BUTTON_B) == LOW || digitalRead(BUTTON_C) == LOW) {
      break;
      }
    }
  }
}


void display_aqi_data() {
  PM25_AQI_Data data;

  display.clearDisplay();

  if (! aqi.read(&data)) {
    display.println("Failed to read AQI data");
    display.display();
    return;
  }

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("AQI Data");

  display.setTextSize(1);
  display.setCursor(0, 25);

  display.print("PM1.0: ");
  display.println(data.pm10_standard);

  display.print("PM2.5: ");
  display.println(data.pm25_standard);


  display.print("PM10: ");
  display.println(data.pm100_standard);

  display.display();
}
