  #define BUTTON_PIN_1 12
  #define BUTTON_PIN_2 13
  
  //MOTOR 1 = storage side motors
  //MOTOR 2 = picking side motors
  #define MOTOR_PWM_1 5
  #define MOTOR_DIR_1 2
  #define MOTOR_PWM_2 18
  #define MOTOR_DIR_2 4
  
  volatile unsigned long pulseInTimeBegin1 = 0;
  volatile unsigned long pulseInTimeEnd1 = 0;
  volatile unsigned long pulseInTimeBegin2 = 0;
  volatile unsigned long pulseInTimeEnd2 = 0;
  
  volatile bool newPulse1 = false;
  volatile bool newPulse2 = false;
  
  // Function to calculate pulse duration with micros() overflow handling
  inline unsigned long calculatePulseDuration(unsigned long start, unsigned long end) {
    return (end >= start) ? (end - start) : (4294967295UL - start + end + 1);
  }
  
  // Interrupt for Channel 1
  void buttonPin1Interrupt() {
    if (digitalRead(BUTTON_PIN_1)) {
      pulseInTimeBegin1 = micros();
    } else {
      pulseInTimeEnd1 = micros();
      newPulse1 = true;
    }
  }
  
  // Interrupt for Channel 2
  void buttonPin2Interrupt() {
    if (digitalRead(BUTTON_PIN_2)) {
      pulseInTimeBegin2 = micros();
    } else {
      pulseInTimeEnd2 = micros();
      newPulse2 = true;
    }
  }
  
  void setup() {
    pinMode(BUTTON_PIN_1, INPUT);
    pinMode(BUTTON_PIN_2, INPUT);
  
    pinMode(MOTOR_PWM_1, OUTPUT);
    pinMode(MOTOR_DIR_1, OUTPUT);
    pinMode(MOTOR_PWM_2, OUTPUT);
    pinMode(MOTOR_DIR_2, OUTPUT);
  
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), buttonPin1Interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), buttonPin2Interrupt, CHANGE);
  }
  
  void loop() {
    static unsigned long pulseDuration1 = 0, pulseDuration2 = 0;

    // Process new pulse for Channel 1
    if (newPulse1) {
      newPulse1 = false;
      pulseDuration1 = calculatePulseDuration(pulseInTimeBegin1, pulseInTimeEnd1);
    }
  
    // Process new pulse for Channel 2
    if (newPulse2) {
      newPulse2 = false;
      pulseDuration2 = calculatePulseDuration(pulseInTimeBegin2, pulseInTimeEnd2);
    }
  
    // Only process valid pulse durations
    if (pulseDuration1 >= 1100 && pulseDuration1 <= 1900 &&
        pulseDuration2 >= 1100 && pulseDuration2 <= 1900) {
  
      // Determine motor direction and write values
      if (pulseDuration1 > 1500 && pulseDuration2 > 1500) {
        //  FORWARD (direction should be high)
        Forward(100,pulseDuration1,pulseDuration2);
      } else if (pulseDuration1 < 1500 && pulseDuration2 < 1500) {
        // BACKWARD (direction should be low)
        reverse(100,pulseDuration1,pulseDuration2);
      } else if (pulseDuration1 > 1500 && pulseDuration2 < 1500) {
        // Right turn
        right(90,100,pulseDuration1,pulseDuration2);
      } else if (pulseDuration1 < 1500 && pulseDuration2 > 1500) {
        // Left turn
        left(90,100,pulseDuration1,pulseDuration2);
       }else if (pulseDuration1 == 1500 && pulseDuration2 == 1500) {
       stop();
      }
   
    }
  }

  void Forward(int speed,unsigned long pulseDuration1, unsigned long pulseDuration2){
    digitalWrite(MOTOR_DIR_1, HIGH); 
    digitalWrite(MOTOR_DIR_2, HIGH);
    int mappedValue1 = map(pulseDuration1, 1500, 1900, 0, speed); //mapped to 130 instead of 255 due to high speed
    int mappedValue2 = map(pulseDuration2, 1500, 1900, 0, speed);

    analogWrite(MOTOR_PWM_1, mappedValue1);
    analogWrite(MOTOR_PWM_2, mappedValue2);
  }

  void reverse(int speed,unsigned long pulseDuration1, unsigned long pulseDuration2){
    digitalWrite(MOTOR_DIR_1, LOW);
    digitalWrite(MOTOR_DIR_2, LOW);
    int mappedValue1 = map(pulseDuration1, 1100, 1500, speed, 0);
    int mappedValue2 = map(pulseDuration2, 1100, 1500,speed, 0);

    analogWrite(MOTOR_PWM_1, mappedValue1);
    analogWrite(MOTOR_PWM_2, mappedValue2);
  }

  void right(int fspeed, int rspeed,unsigned long pulseDuration1, unsigned long pulseDuration2){
    digitalWrite(MOTOR_DIR_1, HIGH);
    digitalWrite(MOTOR_DIR_2, LOW);
    int mappedValue1 = map(pulseDuration1, 1500, 1900, 0, fspeed); // to move the wheel in the reverse direction there is a small delay 
    int mappedValue2 = map(pulseDuration2, 1100, 1500, rspeed,0);  // to compensate this, the mapping values are different

    analogWrite(MOTOR_PWM_1, mappedValue1);
    analogWrite(MOTOR_PWM_2, mappedValue2);
  }

  void left(int fspeed,int rspeed,unsigned long pulseDuration1, unsigned long pulseDuration2){
    digitalWrite(MOTOR_DIR_1, LOW);
    digitalWrite(MOTOR_DIR_2, HIGH);
    int mappedValue1 = map(pulseDuration1, 1100, 1500, rspeed, 0);
    int mappedValue2 = map(pulseDuration2, 1500, 1900, 0, fspeed);

    analogWrite(MOTOR_PWM_1, mappedValue1);
    analogWrite(MOTOR_PWM_2, mappedValue2);
  }

  void stop(){
    analogWrite(MOTOR_PWM_1, 0);
    analogWrite(MOTOR_PWM_2, 0);
  }
  // Needs brakes!! too much momentum
