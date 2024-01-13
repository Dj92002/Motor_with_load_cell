#include <SoftwareSerial.h>
#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include "HX711.h"

// Define the pins for the HX711 module
const int DOUT_PIN = 11;  // DT pin
const int SCK_PIN = 12;   // SCK pin

 float dist = 40.00;
 float rpm_val = 0.00;
float Torque = 0.00;
float power = 0.00;

// Create an instance of the HX711 library
HX711 scale;

LCDWIKI_KBV my_lcd(ILI9486, A3, A2, A1, A0, A4); // Adjust these pins based on your wiring

// digital pin 2 is the hall pin
int hall_pin = 10;
// set number of hall trips for RPM reading (higher improves accuracy)
float hall_thresh = 20.0;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  
  // initialize TFT display (replace with actual initialization)
  my_lcd.Init_LCD();
  my_lcd.Fill_Screen(0x0000); // Set background color to black
  my_lcd.Set_Text_Mode(0);
  my_lcd.Set_Text_Size(4.9);
  my_lcd.Set_Rotation(-3);
  my_lcd.Set_Text_Back_colour(0x00FF);
  my_lcd.Set_Text_colour(0xFFFF); // Set text color to white

   // Initialize the HX711 module
  scale.begin(DOUT_PIN, SCK_PIN);

  
  float calibrationFactor = 449.58; // Replace this with your load cell's calibration factor
  scale.set_scale(calibrationFactor);

  // Tare the scale
  scale.tare();
  
  // make the hall pin an input:
  pinMode(hall_pin, INPUT);
}

void loop() {


  my_lcd.Print_String("RPM: " + String(rpm_val), 30, 50);
  my_lcd.Print_String("Torque: " + String(Torque),30, 150);
  my_lcd.Print_String("Power: " + String(power),30, 250);

  // preallocate values for tach
  float hall_count = 1.0;
  float start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
  while (true) {
    if (digitalRead(hall_pin) == 0) {
      if (on_state == false) {
        on_state = true;
        hall_count += 1.0;
      }
    } else {
      on_state = false;
    }

    if (hall_count >= hall_thresh) {
      break;
    }
  }

  // Read the weight from the load cell
  float weight = scale.get_units(10);  // 10 readings for averaging

  // print information about Time and RPM
  float end_time = micros();
  float time_passed = ((end_time - start) / 1000000.0);
  Serial.print("Time Passed: ");
  Serial.print(time_passed);
  Serial.println("s");
   rpm_val = (hall_count / time_passed) * 60.0;
   Torque = (weight/1000)*dist;
   power = rpm_val*Torque;
  Serial.print("RPM:");
  Serial.println(rpm_val);

  Serial.print("Torque:");
  Serial.println(Torque);

  Serial.print("Power:");
  Serial.println(power);
  
  // Display RPM on the TFT display
  my_lcd.Fill_Rect(10, 10, 320, 40, 0x0000); // Clear the previous RPM value
  my_lcd.Print_String("RPM: " + String(rpm_val), 30, 50);
  my_lcd.Print_String("Torque: " + String(Torque),30, 150);
   my_lcd.Print_String("Power: " + String(power),30, 250);


  // delay in between reads for stability
  delay(1000);

  
}
