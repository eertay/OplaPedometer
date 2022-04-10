/*
  Pedometer
*/

#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

int accread = 0;
float last_ax = 0;
float last_ay = 0;
float last_az = 0;
float acceleration;
int steps;
float xval = 0;
float yval = 0;
float zval = 0;

// LED lights, indexes of pixels

int right1  = 0, right2 = 1, middle = 2, left2 = 3 ,left1 = 4;

uint32_t colorRed = carrier.leds.Color(0,255,0);
uint32_t colorGreen = carrier.leds.Color(255, 0, 0);
uint32_t colorBlue = carrier.leds.Color( 0, 0, 255);
uint32_t colorMagenta = carrier.leds.Color(200, 0, 255);
uint32_t colorCyan = carrier.leds.Color(0, 255, 255);
uint32_t noColor = carrier.leds.Color( 0, 0, 0);


void setup() {
  Serial.begin(9600);
  while (!Serial);

  CARRIER_CASE = false;
  if (!carrier.begin()) {
    Serial.println("Failed to initialize!");

    while (1);
  }

  carrier.display.fillScreen(ST77XX_BLACK);

  Serial.print("Accelerometer sample rate = ");
  Serial.print(carrier.IMUmodule.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in G's");
  Serial.println("X\tY\tZ");

}

void loop() {

  // Initialising x, y, z values for the accelerometer and the gyroscope
  float G_x, G_y, G_z;
  float A_x, A_y, A_z;
  int time = millis();


  if (carrier.IMUmodule.gyroscopeAvailable() && carrier.IMUmodule.accelerationAvailable() && (accread < (time - 100))) {

    
    // Get accelerometer and gyroscope values
    carrier.IMUmodule.readGyroscope(G_x, G_y, G_z);
    carrier.IMUmodule.readAcceleration(A_x, A_y, A_z);
    

    // Calculate acceleration
    acceleration = sqrt((A_x-last_ax)*(A_x-last_ax) + (A_y-last_ay)*(A_y-last_ay) + (A_z-last_az)*(A_z-last_az));

    // Components of acceleration
    xval = sqrt((A_x-last_ax)*(A_x-last_ax));
    yval = sqrt((A_y-last_ay)*(A_y-last_ay));

    // Calculate if a step has been taken    
    if (xval > 0.4 && yval > 0.4) {

      // Update the steps and report to Serial Monitor and Arduino screen
      steps++;
      updateSteps();
      Serial.println("Step!");
      Serial.println("Total Steps: ");
      Serial.print(steps);
      Serial.print('\n');  

      // If the step is to the right, blink the right lights
      if (G_x < -70) {
        Serial.println("Right turn!");
        blink(right1, right2, colorMagenta);
      }

      else if (G_x > 30) {
        Serial.println("Left turn!");
        blink(left1, left2, colorCyan);
    
      }        
   
      delay(300);

    }

    // Every 100 steps, make a buzz to say to the user "Good Job!!!"    
    if  (steps > 0  && (steps % 100) == 0) {
      makeSound();
    }

    /*
    if(acceleration > 30){
          steps++;
          Serial.println("Step!");
          delay(300);
    }
    */

    last_ax = A_x;
    last_ay = A_y;
    last_az = A_z;

    accread = time;


  }

}

void updateSteps() {

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setCursor(25, 60);

  carrier.display.setTextSize(5);
  carrier.display.println("Steps: ");
  carrier.display.print(steps);
  carrier.display.print('\n');

  // makeSound();

}

void blink(int pixel1, int pixel2, uint32_t color) {
  
  carrier.leds.setPixelColor(pixel1, color);
  carrier.leds.setPixelColor(pixel2, color);
  carrier.leds.show();
  delay(100);
  carrier.leds.setPixelColor(pixel1, noColor);
  carrier.leds.setPixelColor(pixel2, noColor);
  carrier.leds.show();

}

void ligthAll() {

  carrier.leds.fill(colorGreen, 0, 5);
  carrier.leds.show();
  delay(100);                        
  carrier.leds.fill(noColor, 0, 5); 
  carrier.leds.show();

}

void makeSound() {

  carrier.Buzzer.sound(500);
  delay(500); 
  carrier.Buzzer.noSound(); 
  
}