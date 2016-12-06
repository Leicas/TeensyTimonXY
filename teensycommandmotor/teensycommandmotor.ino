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
int motor1= 0;
float force1 = 0.0;
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
  if(Serial.available() > 0)
  {
    /*char BUFFER[Serial.available()];
    int index = 0;
    //Serial.println(Serial.available());
    while(Serial.peek() != 10)
    {
    BUFFER[index] = Serial.read();
    index++;
    }
    Serial.read();*/
    String data = Serial.readStringUntil('\r\n');
    sscanf(data.c_str(),"%f",&force1);
    Serial.println(force1);
    motor1 =(int)( abs(force1)/8.0*80.0/100.0*(float)pwmmax+10.0/100.0*(float)pwmmax);
     if(motor1 <= 10.0/100.0*pwmmax)
  {
    motor1 = 10.0/100.0*pwmmax;
  }
  if(motor1 >= 90.0/100.0* pwmmax)
  {
     motor1 = 90.0/100.0*pwmmax;
  }
  }
    /// Motor 1
  if (force1 < 0) {i--;} else {i++;}
  //i--;
  if (i>5) {
  i=0;
  }
  if (i<0) {
    i=5;
  }
  analogWrite(m1p,motor1);
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
      while ((micros()-lasttime) <= 250) {
      digitalWrite(led,LOW);
      }
      lasttime = micros();
  }
  
