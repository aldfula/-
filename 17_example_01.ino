#include <Servo.h>

// Arduino pin assignment

#define PIN_POTENTIOMETER 3 // Potentiometer at Pin A3
#define PIN_IRSENSOR 0 //IR SENSOR at Pin A0
#define PIN_LED   9   // LED active-low
#define PIN_SERVO 10

#define _DUTY_MIN 553  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counter-clockwise position (180 degree)

#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 250.0   // maximum distance to be measured (unit: mm)


#define LOOP_INTERVAL 50   // Loop Interval (unit: msec)

#define _EMA_ALPHA 0.3 // EMA weight of new sample (range: 0 to 1)

float  dist_ema, dist_prev = _DIST_MAX; // unit: mm
        

Servo myservo;
unsigned long last_loop_time;   // unit: msec

void setup()
{
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  pinMode(PIN_LED, OUTPUT);

  dist_prev = _DIST_MIN;
  
  Serial.begin(57600);

}

void loop()
{
  float  dist;  
  unsigned long time_curr = millis();
  int a_value, duty;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  
  a_value = analogRead(PIN_IRSENSOR);  // Read IR Sensor value !!!
  dist = (6762.0/(a_value-9)-4.0)*10.0 - 60.0; // Convert IR sensor value into distance !!!

  if (dist < _DIST_MIN){
    dist = dist_prev;
    digitalWrite(PIN_LED, 1);
  } else if (dist > _DIST_MAX){
    dist = dist_prev;
    digitalWrite(PIN_LED, 1);
  }else{
    dist_prev = dist;
    digitalWrite(PIN_LED, 0);
  }
  dist_ema = (_EMA_ALPHA*dist)+(1-_EMA_ALPHA)*dist_ema; // we need EMA filter here !!!

  // map distance into duty
  if (dist_ema < _DIST_MIN){
    duty = _DUTY_MIN;
  } else if (dist_ema > _DIST_MAX){
    duty = _DUTY_MAX;
  }else{
    duty = (dist_ema-_DIST_MIN)*(_DUTY_MAX - _DUTY_MIN)/150.0+_DUTY_MIN;
  }
  myservo.writeMicroseconds(duty);

  

  // print IR sensor value, distnace, duty !!!
  Serial.print("IR: "); Serial.print(a_value);
  Serial.print(",MIN: "); Serial.print(_DIST_MIN);
  Serial.print(",dist: "); Serial.print(dist);
  Serial.print(",ema: "); Serial.print(dist_ema);
  Serial.print(",servo: "); Serial.print(duty);
  Serial.print(",MAX: "); Serial.print(_DIST_MAX);
  Serial.print("IR: "); Serial.print(a_value);
  Serial.print(" = ");
  Serial.print((a_value / 1024.0) * 5.0);
  Serial.print(" Volt => Duty : ");
  Serial.print(duty);
  Serial.println("usec");
}
