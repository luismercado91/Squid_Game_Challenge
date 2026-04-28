#include <Wire.h>
#include <MPU6050.h>
#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 2048; // change this to fit the number of steps per revolution
const int rolePerMinute = 17;        // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

//set steps and the connection with the MPU
Stepper stepper(stepsPerRevolution, 47, 49, 51, 53);
MPU6050 mpu;

// Variables to store accelerometer readings and calculated angles
float xAccel, yAccel, zAccel;
float pitch, roll;


void setup() 
{
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  Serial.println("Initialize MPU6050");

  stepper.setSpeed(rolePerMinute * 20); // Set the speed of the stepper motor

  // Try to initialize the MPU6050 sensor
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    // If initialization fails, print an error message
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);  // Wait for 500 milliseconds before retrying
  }
  
  checkSettings(); // Call the function to display sensor settings
}

void checkSettings()
{
  Serial.println();

  // Check if the MPU6050 is in sleep mode
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  // Display the clock source of the MPU6050
  Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }

  // Display the accelerometer range setting
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  // Display the accelerometer offsets for X, Y, and Z axes
  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}

void loop()
{
  updateAngle(); // Update the pitch and roll angles

  
  // Print the pitch and roll angles to the Serial Monitor
  Serial.print("Pitch: ");
  Serial.println(pitch);
  Serial.print("Roll: ");
  Serial.println(roll);

  // Check if the absolute value of pitch or roll exceeds 45 degrees
  if(abs(pitch) > 45 || abs(roll) > 45)
  {
    Serial.print("Tilt is greater than 45 degrees, initailizing motor");
    //check if tilt angle is greater than 45
    while (abs(pitch) > 45 || abs(roll) > 45)
    {
      stepper.step(1); // Move the stepper motor one step forward
      delay(1); // Small delay between steps 

      updateAngle(); // Update the pitch and roll angles again
    }
    Serial.print("Tilt is greater than 45 degrees, initailizing motor");
  }
  
  delay(5); // Short delay before repeating the loop
}


void updateAngle()
{ // Read normalized accelerometer values
  Vector normAccel = mpu.readNormalizeAccel();
  
  // Store the accelerometer readings into variables
  xAccel = normAccel.XAxis;
  yAccel = normAccel.YAxis;
  zAccel = normAccel.ZAxis;

  
  // Calculate the pitch angle in degrees
  pitch = atan2(yAccel, sqrt(xAccel * xAccel + zAccel * zAccel)) * (180.0 / M_PI);
  
  // Calculate the roll angle in degrees
  roll = atan2(-xAccel, zAccel) * (180.0 / M_PI);
}
