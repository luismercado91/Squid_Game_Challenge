#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// servo objects
Servo sensorServo;
Servo eliminationServo;

// define pins
#define GREEN_LED_PIN 12
#define RED_LED_PIN 6
#define BUZZER_PIN 7
#define BUTTON_PIN 2
#define TRIG_PIN 4
#define ECHO_PIN 5
#define SENSOR_SERVO_PIN 9
#define ELIMINATION_SERVO_PIN 11

// defin adjustable pins
#define GAME_DURATION 60
#define MOVEMENT_THRESHOLD 5   // we can adjust for sensitivity
#define SERVO_SWEEP_ANGLE 180
#define DEBOUNCE_DELAY 200

// game state variables
volatile int gameState = 0;           // 0: WAITING, 1: RUNNING, 2: ENDED
int timeLeft = GAME_DURATION;         // Game timer in seconds
long lastInterruptTime = 0;

// ultrasonic sensor variables
long duration, distance;
int initialRedLightDistance = 0;

// timing variables
unsigned long greenLightStartTime = 0;
unsigned long redLightStartTime = 0;
unsigned long greenLightDuration;
unsigned long redLightDuration;
bool inGreenLight = false;

// Flags
volatile bool timerFlag = false;
volatile bool buttonPressed = false;


void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // set pin modes
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // attach servos
  sensorServo.attach(SENSOR_SERVO_PIN);   // servi for figure rotation
  eliminationServo.attach(ELIMINATION_SERVO_PIN); // servo for sweeping arm

  //initialize  LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Press Button to");
  lcd.setCursor(0, 1);
  lcd.print("Start Game");

  // sset initial servo positions
  sensorServo.write(0);       // starts facing the board
  eliminationServo.write(0); // starts in ready position

  // turn off LEDs and buzzer
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);

  // Set up interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  Timer1.initialize(1000000); // Timer interrupt every 1 second
  Timer1.attachInterrupt(timerISR);
}


void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    resetGame();              // start or reset the game
  }
  if (gameState == 1) {       
    if (timeLeft > 0) {
      gameCycle();            
    } else {                  // end game when timier reaches 0 count.
      playerEliminated();
      gameOverSequence();
    }
  }
  if (timerFlag) {            // keeps track of time
    timerFlag = false;
    if (gameState == 1 && timeLeft > 0) {
      timeLeft --;
      updateTimerDisplay();
    }
  }
}


void resetGame() {
  timeLeft = GAME_DURATION;
  initialRedLightDistance = 0;
  greenLightStartTime = millis();
  redLightStartTime = 0;
  greenLightDuration = random(1000, 4000); // random times for challenging goal
  redLightDuration = random(1000, 4000);
  inGreenLight = true;
  gameState = 1;

  // reset errthang!
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);

  sensorServo.write(0); // start facing board
  eliminationServo.write(0);

  lcd.clear();
  lcd.print("Game Started!");
  delay(1000);
  updateTimerDisplay();
}


void updateTimerDisplay() {
  lcd.clear();
  lcd.print("Time Left: ");
  lcd.print(timeLeft);
  lcd.print("s  ");
}


void gameCycle() {
  unsigned long currentTime = millis();

  if (inGreenLight) {
    // if in green light state, check if the duration has elapsed
    if (currentTime - greenLightStartTime >= greenLightDuration) {
      // switch to red light
      inGreenLight = false;
      redLightStartTime = currentTime; // Set start time for Red Light
      redLightDuration = random(1000, 4000); // Random duration
      initialRedLightDistance = getDistance();

      if(initialRedLightDistance <= 5)
      { 
        lcd.clear();
        lcd.print("YOU SURVIVED!!!");
        lcd.setCursor(0, 1);
        tone(BUZZER_PIN, 2500, 2000);
        delay(3000);
        gameOverSequence();
 
      } else {
         redLightState();
      }

      // redLightState();
    }
  } else {
    // if in red light state, check if the duration has elapsed
    if (currentTime - redLightStartTime >= redLightDuration) {
      // switch to green light
      inGreenLight = true;
      greenLightStartTime = currentTime; // start time for green light
      greenLightDuration = random(1000, 4000); // radnom duration
      greenLightState();
    } else {
      // during redLightState, continuously check for movement
      redLightState();
    }
  }
}


void greenLightState() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1500, 500);

  sensorServo.write(SERVO_SWEEP_ANGLE); // turn away from board

  lcd.clear();
  lcd.print("Green Light");
  lcd.setCursor(0, 1);
  lcd.print("Move!");

  // reset initial red light distance
  initialRedLightDistance = 0;    
}


void redLightState() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 500, 500);

  sensorServo.write(0); // turn towards board

  lcd.clear();
  lcd.print("Red Light");
  lcd.setCursor(0, 1);
  lcd.print("Stop!");

  // read distance continuosly
  distance = getDistance();

  // movement detection
  if (initialRedLightDistance != 0 && abs(distance - initialRedLightDistance) > MOVEMENT_THRESHOLD) {
    playerEliminated();
    gameOverSequence();
  }
}


long getDistance() {
  // Trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo time
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout after 30ms
  // Calculate the distance
  long distance = duration / 58.2;

  // dsiplay distance on serial screen for debugging
  Serial.print("Distance: ");
  Serial.println(distance);

  return distance;
}


void playerEliminated() {
  tone(BUZZER_PIN, 200, 1000);

  lcd.clear();
  lcd.print("Eliminated!");

  // Sweep the elimination arm
  for (int i = 0; i <= SERVO_SWEEP_ANGLE; i += 4) {
    eliminationServo.write(i);
    delay(20);
  }
}


void gameOverSequence() {
  gameState = 2;

  lcd.clear();
  lcd.print("Game Over!");

  tone(BUZZER_PIN, 400, 2000);
  delay(1000);
  noTone(BUZZER_PIN);

  sensorServo.write(0);
  eliminationServo.write(0);
}


void buttonISR() {
  long interruptTime = millis();
  if (interruptTime - lastInterruptTime > DEBOUNCE_DELAY) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}


void timerISR() {
  timerFlag = true;   //sets timer flag every second
}
