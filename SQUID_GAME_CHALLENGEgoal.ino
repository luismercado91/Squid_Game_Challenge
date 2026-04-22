#include <Wire.h>
#include <MPU6050.h>
#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 2048; // change this to fit the number of steps per revolution
const int rolePerMinute = 17;        // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

//set steps and the connection with MCU
Stepper stepper(stepsPerRevolution, 47, 49, 51, 53);
MPU6050 mpu;

float xAccel, yAccel, zAccel;
float pitch, roll;


void setup() 
{
  Serial.begin(115200);
  Serial.println("Initialize MPU6050");

  stepper.setSpeed(rolePerMinute * 20);

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  checkSettings();
}

void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
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
  
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

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
  updateAnglge();

  Serial.print("Pitch: ");
  Serial.println(pitch);
  Serial.print("Roll: ");
  Serial.println(roll);

  if(abs(pitch) > 45 || abs(roll) > 45)
  {
    Serial.print("Tilt is greater than 45 degrees, initailizing motor");
    // chek if tilt angle is greater than 45
    while (abs(pitch) > 45 || abs(roll) > 45)
    {
      stepper.step(1);
      delay(1);

      updateAnglge();
    }
    Serial.print("Tilt is greater than 45 degrees, initailizing motor");
  }
  
  delay(5);
}


void updateAnglge()
{
  Vector normAccel = mpu.readNormalizeAccel();

  xAccel = normAccel.XAxis;
  yAccel = normAccel.YAxis;
  zAccel = normAccel.ZAxis;

  pitch = atan2(yAccel, sqrt(xAccel * xAccel + zAccel * zAccel)) * (180.0 / M_PI);
  roll = atan2(-xAccel, zAccel) * (180.0 / M_PI);
}
