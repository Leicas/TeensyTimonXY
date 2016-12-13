//variables for HallSensor
int i = 0; //counter to change HallSensor signals
int j = 0;
int k = 0;

const int led = 13;
long lasttime;
const int resolution = 16;
int pwmmax;
/// Motor s ///

const int m31 =  23;
const int m32 = 22;
const int m33 =  21;
const int m3p = 20;


const int m21 =  12;
const int m22 = 11;
const int m23 =  10;
const int m2p = 9;

const int m11 =  6;
const int m12 = 5;
const int m13 =  4;
const int m1p = 3;

int motor1= 0;
float force1 = 0.0;

int motor2= 0;
float force2= 0.0;

int motor3= 0;
float force3= 0.0;

float forcex = 0.0;
float forcey= 0.0;
float rot = 0.0;
////////////////////// Setup //////////////////////
void setup() {
  
  //We set 3 output pins por the 3 sinusoids simulatin the HallSensor:
  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(m13, OUTPUT);
  pinMode(m1p, OUTPUT);
  pinMode(m21, OUTPUT);
  pinMode(m22, OUTPUT);
  pinMode(m23, OUTPUT);
  pinMode(m2p, OUTPUT);
  pinMode(m31, OUTPUT);
  pinMode(m32, OUTPUT);
  pinMode(m33, OUTPUT);
  pinMode(m3p, OUTPUT);
  pinMode(led, OUTPUT);
    analogWriteResolution(resolution);
  pwmmax = pow(2,resolution);  
  Serial.begin(9600);
  Serial.setTimeout(1);
  
  //lcd.begin(20, 4);
  //lcd.clear();  
  //attachInterrupt(0, UpdatePointer, FALLING);
  lasttime = micros();
}


////////////////////// LOOP //////////////////////  
  void loop() {
      digitalWrite(led,HIGH);
  
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
  digitalWrite(m11,i==0||i==1||i==2);
  digitalWrite(m12,i==2||i==3||i==4);
  digitalWrite(m13,i==4||i==5||i==0);
  ////////////////////////////////////////////////////
      /// Motor 2
  ////////////////////////////////////////////////////
  if (force2 < 0) {j--;} else {j++;}
  if (j>5) {
  j=0;
  }
  if (j<0) {
    j=5;
  }
  analogWrite(m2p,motor2);
  digitalWrite(m21,j==0||j==1||j==2);
  digitalWrite(m22,j==2||j==3||j==4);
  digitalWrite(m23,j==4||j==5||j==0);

  ////////////////////////////////////////////////////
      /// Motor 3
  ////////////////////////////////////////////////////
  if (force3 < 0) {k--;} else {k++;}
  if (k>5) {
  k=0;
  }
  if (k<0) {
    k=5;
  }
  analogWrite(m3p,motor3);
  digitalWrite(m31,k==0||k==1||k==2);
  digitalWrite(m32,k==2||k==3||k==4);
  digitalWrite(m33,k==4||k==5||k==0);

  if(Serial.available() > 0)
  {
    
    String data = Serial.readStringUntil('\r\n');
    //String data = "0.00;0.00;0.00";
    char *cstr = &data[0u];
    forcex = atof(strtok(cstr,";"));
    forcey = atof(strtok(NULL,";"));
    rot = atof(strtok(NULL,";"));
    //sscanf(data.c_str(),"%f;%f;%f",&forcex,&forcey,&rot);
    //Serial.println(String(forcex) + " " + String(forcey) + " " + String(rot));
    force1=2./3.*forcex+1./3.*rot;
    force2=-1./3.*(forcex-sqrt(3)*forcey)+1./3.*rot;
    force3=-1./3.*(forcex+sqrt(3)*forcey)+1./3.*rot;
   
    motor1 =(int)( abs(force1)/8.0*80.0/100.0*(float)pwmmax+10.0/100.0*(float)pwmmax);
     if(motor1 <= 10.0/100.0*pwmmax)
  {
    motor1 = 10.0/100.0*pwmmax;
  }
  if(motor1 >= 90.0/100.0* pwmmax)
  {
     motor1 = 90.0/100.0*pwmmax;
  }
   motor2 =(int)( abs(force2)/8.0*80.0/100.0*(float)pwmmax+10.0/100.0*(float)pwmmax);
     if(motor2 <= 10.0/100.0*pwmmax)
  {
    motor2 = 10.0/100.0*pwmmax;
  }
  if(motor2 >= 90.0/100.0* pwmmax)
  {
     motor2 = 90.0/100.0*pwmmax;
  }
   motor3 =(int)( abs(force3)/8.0*80.0/100.0*(float)pwmmax+10.0/100.0*(float)pwmmax);
     if(motor3 <= 10.0/100.0*pwmmax)
  {
    motor3 = 10.0/100.0*pwmmax;
  }
  if(motor3 >= 90.0/100.0* pwmmax)
  {
     motor3 = 90.0/100.0*pwmmax;
  }
  }

  //////////////////////////////////////////////////
       //// Realtime controll
  //////////////////////////////////////////////////
      //to control the teensy frequency
      while ((micros()-lasttime) <= 250) {
      digitalWrite(led,LOW);
      }
      lasttime = micros();
  }
  
