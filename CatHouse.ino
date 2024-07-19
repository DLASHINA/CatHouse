#include <HX711.h>
#include <Stepper.h>

#define MOTION_PIN 13
#define in1 9  
#define in2 10 
#define in3 11 
#define in4 12 

Stepper myStepper(512, in1, in3, in2, in4);

HX711 hx;

int calibration_factor = 114;

void setup() {
  Serial.begin(9600);
  pinMode(MOTION_PIN, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  myStepper.setSpeed(20);

  hx.begin(A1, A0);
  hx.set_scale();
  hx.tare();
  hx.set_scale(calibration_factor);

  Serial.println("PIR Module Test (CTRL-C to exit)");
}

void loop() {

  int Current_State = 0;
  int Previous_State = 0;

  int step_count = -1024;

  Serial.println("Waiting for PIR to settle ...");

  while (digitalRead(MOTION_PIN) == HIGH) {
    Current_State = 0;
  }

  Serial.println("Ready");

  while (true) {
    Current_State = digitalRead(MOTION_PIN);

    if (Current_State == 1 && Previous_State == 0) {
      Serial.println("Motion detected!");
      hx.tare();
      Serial.println(("Tare done! Add weight now..."));
      hx.power_down();
      hx.power_up();
      long val = hx.get_units(5);
      Serial.println(val);
      hx.power_down();
      hx.power_up();
      delay(10);
      if (val > 100) {
        myStepper.step(512);
        delay(2);

        int val_cur = val;
        while (val_cur > val - 100) {
          val_cur = hx.get_units(5);
          Serial.println(val_cur);
          hx.power_down();
          hx.power_up();
          delay(10);
        }

        myStepper.step(-512);
        delay(2);
      } else {
        Serial.println("The cat food is running out!");
      }
      Previous_State = 1;
    } else if (Current_State == 0 && Previous_State == 1) {
      Serial.println("Ready");
      Previous_State = 0;
    }
    delay(10);
  }
}