//variables for HallSensor
int i = 0; //counter to change HallSensor signals

const int led = 13;
long lasttime;
const int resolution = 16;
int pwmmax;
/// Motor 1 ///

const int m11 =  6;
const int m12 = 5;
const int m13 =  4;
const int m1p = 3;

////////////////////// Setup //////////////////////
void setup() {
  
  //We set 3 output pins por the 3 sinusoids simulatin the HallSensor:
  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(m13, OUTPUT);
  pinMode(m1p, OUTPUT);
  pinMode(led, OUTPUT);
    analogWriteResolution(resolution);
  pwmmax = pow(2,resolution);  
  Serial.begin(9600);
  
  //lcd.begin(20, 4);
  //lcd.clear();  
  //attachInterrupt(0, UpdatePointer, FALLING);
  lasttime = micros();
}


////////////////////// LOOP //////////////////////  
  void loop() {
      digitalWrite(led,HIGH);
    /// Motor 1
  i++;
  if (i>5) {
  i=0;
  }
  analogWrite(m1p,pwmmax/3.0*2.0);
   switch (i) {
    case 0:
      digitalWrite(m11, HIGH);  
      digitalWrite(m12, LOW); 
      digitalWrite(m13,HIGH);
      break;
    case 1:
      digitalWrite(m11, HIGH);  
      digitalWrite(m12,LOW);
      digitalWrite(m13,LOW);
      break; 
    case 2:
      digitalWrite(m11,HIGH);  
      digitalWrite(m12,HIGH); 
      digitalWrite(m13,LOW);
      break; 
    case 3:
      digitalWrite(m11, LOW);  
      digitalWrite(m12, HIGH); 
      digitalWrite(m13, LOW);
      break; 
    case 4:
      digitalWrite(m11, LOW);  
      digitalWrite(m12, HIGH); 
      digitalWrite(m13, HIGH);
      break; 
    case 5:
      digitalWrite(m11, LOW);  
      digitalWrite(m12, LOW); 
      digitalWrite(m13, HIGH);
      break;
  }

      //to control the teensy frequency
      while ((micros()-lasttime) <= 1000) {
      digitalWrite(led,LOW);
      }
      lasttime = micros();
  }
  
